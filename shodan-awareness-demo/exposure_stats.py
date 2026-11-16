#!/usr/bin/env python3
"""
exposure_stats.py — Cyber-awareness classroom demo.

Purpose
-------
Show students HOW MANY internet-exposed cameras / RTSP devices exist in a
given city (default: Coimbatore) using Shodan's aggregate `count` API.

This script ONLY counts and summarizes the metadata Shodan already collected.
It never connects to, authenticates against, or streams from any device.
That keeps the whole demo on the legal side of the line: searching public
banners is fine; accessing a device you don't own is not (IT Act 2000 s.43/66
in India, CFAA in the US, Computer Misuse Act in the UK, etc.).

The /shodan/host/count endpoint returns totals + facet breakdowns and does
NOT consume query credits, so it is safe to run on the free Shodan tier.

Usage
-----
    export SHODAN_API_KEY=your_key_here
    python3 exposure_stats.py                 # defaults to Coimbatore
    python3 exposure_stats.py --city Chennai
    python3 exposure_stats.py --city Coimbatore --country IN

Get a free key at https://account.shodan.io/register
"""

import argparse
import json
import os
import sys
import urllib.parse
import urllib.request

COUNT_URL = "https://api.shodan.io/shodan/host/count"

# Facets tell Shodan to break the total down by these fields.
# Format: "field:number_of_top_values"
FACETS = "port:10,org:10,product:10,city:5"


def shodan_count(api_key: str, query: str, facets: str = FACETS) -> dict:
    """Call the credit-free count endpoint and return the parsed JSON."""
    params = urllib.parse.urlencode(
        {"key": api_key, "query": query, "facets": facets}
    )
    url = f"{COUNT_URL}?{params}"
    try:
        with urllib.request.urlopen(url, timeout=30) as resp:
            return json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as e:
        body = e.read().decode("utf-8", "replace")
        sys.exit(f"[!] Shodan API error {e.code}: {body}")
    except urllib.error.URLError as e:
        sys.exit(f"[!] Network error reaching Shodan: {e.reason}")


def print_facet(title: str, facet_list: list) -> None:
    print(f"\n  {title}")
    if not facet_list:
        print("    (no data)")
        return
    for row in facet_list:
        print(f"    {str(row['value']):<28} {row['count']:>8,}")


def run_query(api_key: str, label: str, query: str) -> int:
    print("\n" + "=" * 60)
    print(f"QUERY: {label}")
    print(f"  Shodan filter: {query}")
    print("=" * 60)
    data = shodan_count(api_key, query)
    total = data.get("total", 0)
    print(f"\n  >>> TOTAL EXPOSED DEVICES: {total:,}")
    facets = data.get("facets", {})
    print_facet("By port:", facets.get("port", []))
    print_facet("By organization / ISP:", facets.get("org", []))
    print_facet("By device / product:", facets.get("product", []))
    return total


def main() -> None:
    ap = argparse.ArgumentParser(description="Shodan exposure stats (count only)")
    ap.add_argument("--city", default="Coimbatore", help="City to search")
    ap.add_argument("--country", default="IN", help="ISO country code (default IN)")
    args = ap.parse_args()

    api_key = os.environ.get("SHODAN_API_KEY")
    if not api_key:
        sys.exit(
            "[!] Set your API key first:\n"
            "    export SHODAN_API_KEY=your_key_here\n"
            "    (free key: https://account.shodan.io/register)"
        )

    loc = f'city:"{args.city}" country:{args.country}'

    print("#" * 60)
    print("# CYBER-AWARENESS DEMO — exposure counts only, no access")
    print(f"# Location: {args.city}, {args.country}")
    print("#" * 60)

    queries = [
        ("All RTSP services (port 554)", f"{loc} port:554"),
        ("Webcam / camera devices", f'{loc} device:webcam'),
        ("Devices exposing a screenshot", f"{loc} has_screenshot:true"),
        ("Open Telnet (classic IoT weakness)", f"{loc} port:23"),
    ]

    grand_total = 0
    for label, q in queries:
        grand_total += run_query(api_key, label, q)

    print("\n" + "#" * 60)
    print("# TEACHING POINT")
    print("#" * 60)
    print(
        f"""
  These devices in {args.city} are reachable from anywhere on the
  internet RIGHT NOW. We counted them without touching a single one.

  Do NOT open any of these feeds — 'no password' is not 'permission'.
  Viewing a stranger's camera is unauthorized access and is illegal.

  The fix (share with students):
    - Change default passwords on every device
    - Disable UPnP on the home router
    - Never port-forward a camera; use a VPN or the vendor cloud
    - Put IoT gadgets on a separate guest network
"""
    )


if __name__ == "__main__":
    main()
