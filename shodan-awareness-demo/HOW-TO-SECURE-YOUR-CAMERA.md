# How Your Camera Ends Up on the Internet — and How to Stop It

A plain-language guide for students. Based on a real Shodan scan of
**Coimbatore**, which found **1,160 RTSP camera devices, 162 open Telnet
services, and 28 exposed remote desktops** reachable by anyone, anywhere.

---

## 1. What actually happened

Nobody "hacked" these cameras. The owners exposed them by accident:

1. They bought an IP camera / DVR and plugged it in.
2. They (or an installer, or the router's **UPnP** feature) opened a **port**
   so they could watch the feed from their phone while away.
3. That port is now open to the *entire internet*, not just their phone.
4. They left the **default password** (or no password) in place.

Search engines like Shodan continuously scan every internet address and
record what answers. Your camera answered. Now it's in a searchable index.

> **Key idea:** An open port + a default password = a camera anyone can watch.
> No hacking skill required.

---

## 2. Why "no password" is still not "free to watch"

This is the most important lesson. Even when a camera has **no login at all**,
opening its feed is **illegal**:

- **India — IT Act 2000, Sections 43 & 66:** unauthorized access to a computer
  resource, punishable by fine and imprisonment.
- **US — CFAA**, **UK — Computer Misuse Act**, and equivalents worldwide.

Access without permission is the crime. An unlocked door is not an invitation.
So we *count* and *study* the exposure — we never open someone else's feed.

---

## 3. Is MY camera exposed? (test only what you own)

You may test a device **you own**:

1. **Find your camera's local IP** (router device list or the camera app),
   e.g. `192.168.1.50`.
2. In **VLC → Media → Open Network Stream**, try your model's RTSP URL:
   ```
   rtsp://user:pass@192.168.1.50:554/stream1
   ```
   (Exact path varies by brand — check the manual: Hikvision, Dahua, CP Plus,
   etc. all differ.)
3. **Look up your own public IP on Shodan:** visit https://www.shodan.io and
   search `net:YOUR.PUBLIC.IP`. If your camera shows up, the world can see it.
   (Find your public IP by searching "what is my IP" in a browser.)

If it appears on Shodan → fix it using the checklist below.

---

## 4. The fix — do all of these

### On the camera / DVR
- [ ] **Change the default password** to a long, unique one. This alone stops
      the vast majority of exposure.
- [ ] Update the firmware (old firmware = known holes).
- [ ] Disable any "cloud" or "P2P" feature you don't actually use.

### On the router (the real gatekeeper)
- [ ] **Turn OFF UPnP.** It silently opens ports for devices — the #1 cause of
      accidental exposure.
- [ ] **Remove any port-forwarding rules** pointing to the camera (port 554,
      8000, 80, etc.).
- [ ] Change the router's own admin password.

### The right way to watch your camera from outside
- [ ] Use the manufacturer's **cloud app** (relays video without opening ports), **or**
- [ ] Set up a **VPN** into your home network, then connect to the camera as if
      you were at home. Never expose the camera directly.

### Bonus hardening
- [ ] Put cameras and IoT gadgets on a **separate guest network / VLAN**, so a
      compromised camera can't reach your laptop or phone.
- [ ] **Disable Telnet (port 23)** and use SSH if remote admin is needed.

---

## 5. Why this matters — real consequences

- **Mirai botnet (2016):** hijacked hundreds of thousands of cameras and DVRs
  using default passwords, then took down large parts of the internet.
- **Camera-aggregator websites** have scraped and published thousands of open
  home and baby-monitor feeds — a serious privacy violation for the victims.
- **Exposed RDP** (the 28 remote desktops we found) is a top entry point for
  ransomware.

The people in these statistics are ordinary families. The fix takes ten
minutes. Spreading that ten-minute habit is the whole point of this lesson.

---

## 6. The rules we followed (and you must too)

✅ Search, count, and analyze public exposure data
✅ Test only devices you personally own
✅ Look up your own public IP

❌ Never open, record, or share a stranger's feed
❌ Never guess or enter passwords on a device you don't own
❌ Never treat "no password" as permission
