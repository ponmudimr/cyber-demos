# Camera Exposure by Vendor — Coimbatore

**Source:** Shodan aggregate `count` API, 2026-09-28
**Scope:** Internet-exposed devices in Coimbatore, India
**Method:** Counts only — no device was accessed, no IPs are listed.

---

## Headline numbers

- **1,160** RTSP (port 554) camera/DVR devices reachable from the open internet
- **162** open Telnet (port 23) services
- **28** exposed remote desktops (RDP)

## By vendor

| Vendor | Exposed devices | Origin | Regulatory note |
|---|---:|---|---|
| **Hikvision** | **591** | China | Banned from US federal networks — NDAA §889 (2019), FCC |
| **Dahua** | **139** | China | Banned from US federal networks — NDAA §889 (2019), FCC |
| **CP Plus** | **49** | India (Chinese OEM hardware) | Widely rebadged OEM units |
| **TP-Link** | **47** | China | Common consumer networking/camera vendor |

> Roughly **825 of ~1,160** exposed camera devices are Hikvision, Dahua, or
> CP Plus. A handful of vendors account for the large majority of the exposure.

*(Note: `CP Plus` and `CP-Plus` match the same 49 devices — not additive.
Plain RTSP banners rarely self-report a product string, so per-vendor keyword
counts give a truer market picture than the raw port:554 product facet.)*

---

## How to read this (for students)

**The regulatory concern is real and citable.** The US **NDAA Section 889
(2019)** and subsequent **FCC** action barred Hikvision and Dahua from federal
systems over security and surveillance concerns. That is a concrete fact, not
an opinion.

**But exposure is a shared failure, not just a vendor flag:**

1. **Vendor** ships cheap hardware with weak default settings.
2. **Owner** never changes the default password.
3. **Router / UPnP** auto-opens a port to the whole internet.
4. **ISP** assigns a public IP, so the device is globally reachable.

Remove any one of those and the camera is safe. That's the empowering part:
users can fix steps 2 and 3 themselves in ten minutes.

**It is not a "one country" problem.** The same misconfiguration exposes Axis,
Reolink, Foscam and others in other markets — these vendors simply dominate
*this* local market. The lesson is *insecure defaults + no user action*, which
is universal.

---

## The fix

See **HOW-TO-SECURE-YOUR-CAMERA.md** for the full student checklist:
change default passwords, disable UPnP, remove port-forwards, use a VPN or
vendor cloud for remote viewing, and isolate IoT on a separate network.

---

## What this document deliberately excludes

- **No device IP addresses.** A list of reachable vulnerable cameras is a
  target list; sharing it would enable the exact unauthorized access this
  lesson warns against. Vendor *counts* make the point without endangering
  anyone or crossing legal lines (IT Act 2000 s.43/66).
