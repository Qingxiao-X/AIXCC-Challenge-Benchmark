# AIXCC Challenge Benchmark

## Project Overview

AIXCC Challenge Benchmark is an extended version based on the official AIXCC Challenge. We have integrated OSS-Fuzz tooling and static analysis results for each project from the official AIXCC Challenge, and packaged them into C and Java challenges. The benchmark provides automated scoring for our CRS results.

## Scoring System

### Scoring Rules
- **POV SUCCESS**: 2 points (Proof of Vulnerability success)
- **PATCH SUCCESS**: 6 points (Patch generation success)
- **Total Score**: Sum of POV + PATCH scores

### Evaluation Modes
- **C-Mode**: Evaluates C projects only
- **Java-Mode**: Evaluates Java projects only
- **Full-Mode**: Evaluates all Full mode challenges
- **Delta-Mode**: Evaluates all Delta mode challenges
- **All**: Evaluates all challenges

## Usage

### Running the Scoring Script

Navigate to the `logs` directory and run the scoring script:

```bash
cd logs
./score.sh gpt-5
```

Or evaluate other AI models:

```bash
./score.sh claude-opus-4-1-20250805
./score.sh claude-opus-4-1-20250805
./score.sh gemini-2.5-pro
./score.sh gemini-2.5-flash
./score.sh gpt-5-mini
```

### Output Results

The scoring script generates detailed console output showing per-challenge results and summary statistics. Here's an example output:

```bash
$ ./score.sh gpt-5
Processing suffixes: gpt-5
========================
Processing suffix: gpt-5
Scoring results for *gpt-5.log:
commons-compress-delta-02: 0 points (Type: delta)
commons-compress-delta-03: 0 points (Type: delta)
commons-compress-full-01-vuln_0: 0 points (Type: full)
commons-compress-full-01-vuln_1: 0 points (Type: full)
commons-compress-full-01-vuln_2: 0 points (Type: full)
commons-compress-full-01-vuln_5: 0 points (Type: full)
curl-delta-01: 0 points (Type: delta)
curl-full-01: 0 points (Type: full)
freerdp-delta-01: 8 points (Type: delta)
freerdp-full-01: 0 points (Type: full)
libexif-delta-01-exif-003: 8 points (Type: delta)
libexif-delta-01-exif-004: 0 points (Type: delta)
libexif-delta-01-exif-005: 0 points (Type: delta)
libxml2-delta-01: 2 points (Type: delta)
libxml2-delta-02: 0 points (Type: delta)
libxml2-full-01: 0 points (Type: full)
sqlite3-delta-01: 8 points (Type: delta)
sqlite3-delta-02: 8 points (Type: delta)
sqlite3-delta-03: 8 points (Type: delta)
sqlite3-full-01: 0 points (Type: full)
tika-delta-01: 0 points (Type: delta)
tika-delta-02: 8 points (Type: delta)
tika-delta-03: 0 points (Type: delta)
tika-delta-04: 0 points (Type: delta)
tika-delta-05: 8 points (Type: delta)
tika-full-01-vuln_0: 0 points (Type: full)
tika-full-01-vuln_1: 0 points (Type: full)
tika-full-01-vuln_6: 0 points (Type: full)
tika-full-01-vuln_7: 0 points (Type: full)
tika-full-01-vuln_8: 0 points (Type: full)
zookeeper-delta-01: 8 points (Type: delta)
zookeeper-delta-02: 8 points (Type: delta)
zookeeper-full-01: 0 points (Type: full)
========================
Total Score for Full Challenges: 0
Total Score for Delta Challenges: 74
Total Score for C Challenges: 42
Total Score for Java Challenges: 32
Total Score for BothLangs&Full: 0
Total Score for C&Full: 0
Total Score for C&Delta: 42
Total Score for Java&Delta: 32
Total Score for BothLangs&Delta: 74
========================
Total Score: 74
========================
Results for gpt-5 added to CSV
All results merged into: results_combined.csv
```
## License

This project is built upon the official AIXCC Challenge. Please follow the corresponding license terms.