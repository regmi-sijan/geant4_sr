"""Summarize calorimeter response and create lightweight diagnostic plots."""

import argparse
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


INVISIBLE_PDGS = {12, -12, 14, -14, 16, -16}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--truth", type=Path, default=Path("output/truth.csv"))
    parser.add_argument("--hits", type=Path, default=Path("output/hits.csv"))
    parser.add_argument("--vertices", type=Path, default=Path("output/vertices.csv"))
    parser.add_argument("--output-dir", type=Path, default=Path("output/analysis"))
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    truth = pd.read_csv(args.truth)
    hits = pd.read_csv(args.hits)
    vertices = pd.read_csv(args.vertices).set_index("event_id")

    momentum = np.sqrt(truth.px_gev**2 + truth.py_gev**2 + truth.pz_gev**2)
    theta = np.arccos(np.clip(truth.pz_gev / momentum.replace(0, np.nan), -1, 1))
    truth["eta"] = -np.log(np.tan(theta / 2))
    visible = truth[(~truth.pdg.isin(INVISIBLE_PDGS)) & (truth.eta.abs() < 1.1)]
    truth_event = visible.groupby("event_id").energy_gev.sum().rename("truth_visible_gev")

    hit_event = (
        hits.groupby(["event_id", "detector"]).energy_mev.sum().unstack(fill_value=0) / 1000.0
    )
    hit_event.columns = [f"{name}_gev" for name in hit_event.columns]
    # Start from the vertex table so every generated event remains represented,
    # including events with no visible central truth particles or no hits.
    summary = vertices.join(truth_event, how="left").join(hit_event, how="left")
    summary["truth_visible_gev"] = summary.truth_visible_gev.fillna(0)
    for column in ("emcal_gev", "hcal_gev"):
        if column not in summary:
            summary[column] = 0.0
        else:
            summary[column] = summary[column].fillna(0)
    summary["calorimeter_gev"] = summary.emcal_gev + summary.hcal_gev
    summary["response"] = summary.calorimeter_gev / summary.truth_visible_gev.replace(0, np.nan)
    summary.to_csv(args.output_dir / "event_summary.csv")

    fig, ax = plt.subplots(figsize=(6, 5))
    accepted = summary[summary.truth_visible_gev > 0]
    ax.scatter(accepted.truth_visible_gev, accepted.calorimeter_gev, s=14, alpha=0.7)
    limit = max(summary.truth_visible_gev.max(), summary.calorimeter_gev.max(), 1)
    ax.plot([0, limit], [0, limit], "--", color="0.4", label="ideal response")
    ax.set(xlabel="Visible truth energy [GeV]", ylabel="Deposited calorimeter energy [GeV]")
    ax.legend()
    fig.tight_layout()
    fig.savefig(args.output_dir / "truth_vs_deposit.png", dpi=160)
    plt.close(fig)

    finite = summary.response[np.isfinite(summary.response)]
    fig, ax = plt.subplots(figsize=(6, 4))
    ax.hist(finite, bins=30, histtype="stepfilled", alpha=0.75)
    ax.set(xlabel="(EMCal + HCal deposit) / visible truth energy", ylabel="Events")
    fig.tight_layout()
    fig.savefig(args.output_dir / "response.png", dpi=160)
    plt.close(fig)

    print(summary.describe().to_string())


if __name__ == "__main__":
    main()
