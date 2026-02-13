# Frequency Attack (quick note)

Goal: recover hints about the plaintext (or key) from a cipher by analyzing **letter frequencies**.

## Principle
Natural languages have typical letter distributions (e.g., in French: E, A, S, I, N…).
If a text is encrypted with a **monoalphabetic substitution** (or similar), the ciphertext often keeps frequency patterns.

## Steps
1. Count occurrences of each letter in the ciphertext
2. Sort letters by frequency
3. Compare with the target language distribution
4. Try substitutions and validate using common words/patterns

## Limits
- Works poorly on short texts
- Punctuation / mixed languages can distort results
- Not effective against polyalphabetic ciphers (e.g., Vigenère) without additional techniques
