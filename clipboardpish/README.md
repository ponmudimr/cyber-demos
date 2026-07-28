# Crack-Time Console

A live password entropy and crack-time visualizer, built for the `cyber-demos` awareness guide.

Type any password into the console prompt and watch the estimated crack time update in real time, calculated against several real-world attacker speeds (throttled online login, offline slow-hash, offline fast-hash/GPU, nation-state cluster). Also flags passwords found on common leaked-password lists, since those get tried in seconds regardless of length.

**Nothing is sent anywhere.** All calculations run client-side in plain JavaScript — no network calls, no storage, no logging.

## Why

Ties directly into the guide's [Passwords and Authentication](../../README.md#passwords-and-authentication) section: length beats complexity, and a good passphrase can outlast a short complex password by orders of magnitude. Seeing that gap as a live number tends to land better than reading it as a sentence.

## Run it

Just open `index.html` in a browser — no build step, no dependencies.

## How the math works

- **Charset size**: detects which character classes are present (lowercase, uppercase, digits, symbols) and sums their sizes.
- **Entropy (bits)** = `length × log2(charset size)`.
- **Crack time** = `(2^entropy / 2) / guesses-per-second`, averaged over the expected search space.
- Common leaked passwords are checked against a small sample list and flagged as instantly crackable, since real attackers try those first.

This is a simplified educational model, not a rigorous cracking-time predictor — real attacks use dictionaries, pattern rules, and prior breach data that make many "complex-looking" passwords far weaker than raw entropy suggests.
