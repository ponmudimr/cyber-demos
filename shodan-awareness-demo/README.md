# Shodan Cyber-Awareness Demo

A classroom demo showing students how many internet-connected cameras and
IoT devices are exposed — **without accessing any of them.**

## The one rule

> **Searching and counting is legal. Accessing is not.**
> "No password" does **not** mean "authorized." Opening a stranger's camera
> feed is unauthorized access under the IT Act 2000 (s.43/66), the CFAA, the
> Computer Misuse Act, and similar laws. This demo never connects to a device.

## What the script does

`exposure_stats.py` uses Shodan's `count` API (free tier, no query credits) to
report **aggregate statistics** for a city:

- Total RTSP / webcam / exposed devices
- Breakdown by port, ISP, and device model

It reads only the banner metadata Shodan already collected. It opens no streams.

## Run it

```bash
export SHODAN_API_KEY=your_key_here      # free key: https://account.shodan.io/register
python3 exposure_stats.py                # defaults to Coimbatore, IN
python3 exposure_stats.py --city Chennai
```

## Suggested lesson flow

1. **Run the script** — show the real count for your own city. That local
   number is the hook: *"X unsecured cameras, right here, right now."*
2. **Show scale safely** — on shodan.io, search `has_screenshot:true` to view
   Shodan's *own* crawled thumbnails (their stored data, not live access).
3. **Best demo: use your own camera** — expose a cheap IP camera with default
   settings, show it becomes findable, then secure it live in front of the class.
4. **The fix** — walk through the defensive checklist below.

## Defensive checklist for students

- Change default passwords on every device (cameras, routers, DVRs)
- Disable UPnP on the home router
- Never port-forward a camera — use a VPN or the vendor's cloud relay
- Put IoT devices on a separate guest network / VLAN
- Look up your *own* public IP on Shodan to see what you expose

## What this demo deliberately does NOT include

- No RTSP links to other people's cameras
- No password guessing or default-credential testing
- No connecting to, recording, or screenshotting anyone's live feed

Those are the exact behaviors the lesson teaches students to avoid.
