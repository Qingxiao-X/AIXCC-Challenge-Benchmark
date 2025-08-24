#!/bin/bash

root_dir="$1"

if [[ ! -d "$root_dir" ]]; then
  echo "Usage: $0 /path/to/challenges-root-dir"
  exit 1
fi

cd "$root_dir" || exit 1

for dir in local-test-*; do
  [[ -d "$dir" ]] || continue

  echo "==> Cleaning $dir"
  cd "$dir" || continue

  challenge_prefix=$(echo "$dir" | sed -E 's/local-test-([a-zA-Z0-9-]+)-(delta|full).*/\1/')

  afc_pattern="afc-${challenge_prefix}-*"

  sudo rm -rf \
    ${afc_pattern} \
    ap* \
    xp* \
    *corpus \
    fuzz-tooling/build/ \
    patch* \
    vul_fun* \
    codeql_databases/ \
    detect_timeout_crash

  cd ..
done

echo "✅ Clean complete."

