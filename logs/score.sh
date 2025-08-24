#!/bin/bash

# Check if arguments are provided
if [ $# -lt 1 ]; then
    echo "Usage: $0 <suffix1> [suffix2] [suffix3] ..."
    echo "Example: $0 claude-opus-4-1-20250805 claude-sonnet-4-20250514 gemini-2.5-flash"
    exit 1
fi

# Store all suffixes in an array
suffixes=("$@")
echo "Processing suffixes: ${suffixes[*]}"

# Create CSV output file
csv_file="results_combined.csv"
echo "Mode,Model,TotalScore,POV Found,Patch Found" > "$csv_file"

# Function to get challenge list as space-separated string
get_challenge_list() {
    local -n array_ref=$1
    local challenges=""
    for challenge in "${!array_ref[@]}"; do
        if [ -z "$challenges" ]; then
            challenges="$challenge"
        else
            challenges="$challenges $challenge"
        fi
    done
    echo "$challenges"
}

# Function to merge arrays and return space-separated string with deduplication
merge_challenge_arrays() {
    local merged_challenges=""
    declare -A seen_challenges
    
    for array_name in "$@"; do
        local -n array_ref="$array_name"
        for challenge in "${!array_ref[@]}"; do
            if [[ -z "${seen_challenges[$challenge]}" ]]; then
                seen_challenges["$challenge"]=1
                if [ -z "$merged_challenges" ]; then
                    merged_challenges="$challenge"
                else
                    merged_challenges="$merged_challenges $challenge"
                fi
            fi
        done
    done
    echo "$merged_challenges"
}

# Declare arrays outside the loop
declare -A combination_scores
declare -A pov_success_challenges
declare -A patch_success_challenges

declare -A pov_success_C_full
declare -A pov_success_C_delta
declare -A pov_success_Java_full
declare -A pov_success_Java_delta
declare -A pov_success_Both_full
declare -A pov_success_Both_delta
declare -A pov_success_C_Both
declare -A pov_success_Java_Both
declare -A pov_success_Both_Both

declare -A patch_success_C_full
declare -A patch_success_C_delta
declare -A patch_success_Java_full
declare -A patch_success_Java_delta
declare -A patch_success_Both_full
declare -A patch_success_Both_delta
declare -A patch_success_C_Both
declare -A patch_success_Java_Both
declare -A patch_success_Both_Both

# Process each suffix
for suffix in "${suffixes[@]}"; do
    echo "========================"
    echo "Processing suffix: $suffix"
    echo "Scoring results for *$suffix.log:"
    
    # Reset all scores and arrays
    full_score=0
    delta_score=0
    c_score=0
    java_score=0
    
    # Clear all associative arrays
    unset combination_scores
    unset pov_success_challenges
    unset patch_success_challenges
    
    unset pov_success_C_full
    unset pov_success_C_delta
    unset pov_success_Java_full
    unset pov_success_Java_delta
    unset pov_success_Both_full
    unset pov_success_Both_delta
    unset pov_success_C_Both
    unset pov_success_Java_Both
    unset pov_success_Both_Both
    
    unset patch_success_C_full
    unset patch_success_C_delta
    unset patch_success_Java_full
    unset patch_success_Java_delta
    unset patch_success_Both_full
    unset patch_success_Both_delta
    unset patch_success_C_Both
    unset patch_success_Java_Both
    unset patch_success_Both_Both
    
    # Redeclare arrays
    declare -A combination_scores
    declare -A pov_success_challenges
    declare -A patch_success_challenges
    
    declare -A pov_success_C_full
    declare -A pov_success_C_delta
    declare -A pov_success_Java_full
    declare -A pov_success_Java_delta
    declare -A pov_success_Both_full
    declare -A pov_success_Both_delta
    declare -A pov_success_C_Both
    declare -A pov_success_Java_Both
    declare -A pov_success_Both_Both
    
    declare -A patch_success_C_full
    declare -A patch_success_C_delta
    declare -A patch_success_Java_full
    declare -A patch_success_Java_delta
    declare -A patch_success_Both_full
    declare -A patch_success_Both_delta
    declare -A patch_success_C_Both
    declare -A patch_success_Java_Both
    declare -A patch_success_Both_Both

    # Define the C and Java challenge patterns
    c_challenges=("curl" "freerdp" "libexif" "libxml" "sqlite")
    java_challenges=("commons-compress" "tika" "zookeeper")

    # Loop through matching log files
    for logfile in ./*"$suffix".log; do
        # Skip if no matching file
        [[ -f "$logfile" ]] || continue

        score=0
        filename=$(basename "$logfile")
        challenge_name=$(echo "$filename" | cut -d'-' -f1)  # Extract challenge type from filename

        # Remove the dynamic suffix and .log from the filename
        challenge_base=$(echo "$filename" | sed -E "s/(-$suffix|\.log)//g")

        # Check if it's a full or delta challenge
        if [[ "$filename" =~ "full" ]]; then
            challenge_type="full"
        elif [[ "$filename" =~ "delta" ]]; then
            challenge_type="delta"
        else
            challenge_type="unknown"
        fi

        # Determine if the challenge is C or Java
        is_c_challenge=false
        is_java_challenge=false
        for c_ch in "${c_challenges[@]}"; do
            if [[ "$filename" =~ "$c_ch" ]]; then
                is_c_challenge=true
                break
            fi
        done
        for java_ch in "${java_challenges[@]}"; do
            if [[ "$filename" =~ "$java_ch" ]]; then
                is_java_challenge=true
                break
            fi
        done

        # Check for POV SUCCESS and PATCH SUCCESS flags and calculate score
        pov_success_found=false
        patch_success_found=false

        if grep -q 'POV SUCCESS' "$logfile"; then
            pov_success_found=true
            score=$((score + 2))
        fi

        if grep -q 'PATCH SUCCESS' "$logfile"; then
            patch_success_found=true
            score=$((score + 6))
        fi

        # Add score to respective totals
        if [[ "$challenge_type" == "full" ]]; then
            full_score=$((full_score + score))
        elif [[ "$challenge_type" == "delta" ]]; then
            delta_score=$((delta_score + score))
        fi

        if $is_c_challenge; then
            c_score=$((c_score + score))
        fi

        if $is_java_challenge; then
            java_score=$((java_score + score))
        fi

        # Track combinations for POV and PATCH successes
        if $is_c_challenge && [[ "$challenge_type" == "full" ]]; then
            combination_scores["C&Full"]=$((combination_scores["C&Full"] + score))
            if [[ $score == 2 || $score == 8 ]]; then
                pov_success_C_full["$challenge_base"]=1
            fi
            if [[ $score == 6 || $score == 8 ]]; then
                patch_success_C_full["$challenge_base"]=1
            fi
        fi
        if $is_c_challenge && [[ "$challenge_type" == "delta" ]]; then
            combination_scores["C&Delta"]=$((combination_scores["C&Delta"] + score))
            if [[ $score == 2 || $score == 8 ]]; then
                pov_success_C_delta["$challenge_base"]=1
            fi
            if [[ $score == 6 || $score == 8 ]]; then
                patch_success_C_delta["$challenge_base"]=1
            fi
        fi
        if $is_c_challenge && [[ "$challenge_type" =~ "full|delta" ]]; then
            combination_scores["C&Both"]=$((combination_scores["C&Both"] + score))
            if [[ $score == 2 || $score == 8 ]]; then
                pov_success_C_Both["$challenge_base"]=1
            fi
            if [[ $score == 6 || $score == 8 ]]; then
                patch_success_C_Both["$challenge_base"]=1
            fi
        fi

        if $is_java_challenge && [[ "$challenge_type" == "full" ]]; then
            combination_scores["Java&Full"]=$((combination_scores["Java&Full"] + score))
            if [[ $score == 2 || $score == 8 ]]; then
                pov_success_Java_full["$challenge_base"]=1
            fi
            if [[ $score == 6 || $score == 8 ]]; then
                patch_success_Java_full["$challenge_base"]=1
            fi
        fi
        if $is_java_challenge && [[ "$challenge_type" == "delta" ]]; then
            combination_scores["Java&Delta"]=$((combination_scores["Java&Delta"] + score))
            if [[ $score == 2 || $score == 8 ]]; then
                pov_success_Java_delta["$challenge_base"]=1
            fi
            if [[ $score == 6 || $score == 8 ]]; then
                patch_success_Java_delta["$challenge_base"]=1
            fi
        fi
        if $is_java_challenge && [[ "$challenge_type" =~ "full|delta" ]]; then
            combination_scores["Java&Both"]=$((combination_scores["Java&Both"] + score))
            if [[ $score == 2 || $score == 8 ]]; then
                pov_success_Java_Both["$challenge_base"]=1
            fi
            if [[ $score == 6 || $score == 8 ]]; then
                patch_success_Java_Both["$challenge_base"]=1
            fi
        fi

        if ($is_c_challenge || $is_java_challenge) && [[ "$challenge_type" == "full" ]]; then
            combination_scores["BothLangs&Full"]=$((combination_scores["BothLangs&Full"] + score))
            if [[ $score == 2 || $score == 8 ]]; then
                pov_success_Both_full["$challenge_base"]=1
            fi
            if [[ $score == 6 || $score == 8 ]]; then
                patch_success_Both_full["$challenge_base"]=1
            fi
        fi
        if ($is_c_challenge || $is_java_challenge) && [[ "$challenge_type" == "delta" ]]; then
            combination_scores["BothLangs&Delta"]=$((combination_scores["BothLangs&Delta"] + score))
            if [[ $score == 2 || $score == 8 ]]; then
                pov_success_Both_delta["$challenge_base"]=1
            fi
            if [[ $score == 6 || $score == 8 ]]; then
                patch_success_Both_delta["$challenge_base"]=1
            fi
        fi
        if ($is_c_challenge || $is_java_challenge) && [[ "$challenge_type" =~ "full|delta" ]]; then
            combination_scores["BothLangs&Both"]=$((combination_scores["BothLangs&Both"] + score))
            if [[ $score == 2 || $score == 8 ]]; then
                pov_success_Both_Both["$challenge_base"]=1
            fi
            if [[ $score == 6 || $score == 8 ]]; then
                patch_success_Both_Both["$challenge_base"]=1
            fi
        fi

        # Print per-file results
        echo "$challenge_base: $score points (Type: $challenge_type)"
    done

    echo "========================"
    echo "Total Score for Full Challenges: $full_score"
    echo "Total Score for Delta Challenges: $delta_score"
    echo "Total Score for C Challenges: $c_score"
    echo "Total Score for Java Challenges: $java_score"

    # Display combination scores and list challenges for POV and PATCH success
    for combination in "${!combination_scores[@]}"; do
        echo "Total Score for $combination: ${combination_scores[$combination]}"
        
        # List challenges for POV SUCCESS
        echo "Challenges for POV SUCCESS in $combination:"
        case "$combination" in
            "C&Full")  
                for challenge in "${!pov_success_C_full[@]}"; do echo "  - $challenge"; done
                ;;
            "C&Delta")
                for challenge in "${!pov_success_C_delta[@]}"; do echo "  - $challenge"; done
                ;;
            "Java&Full")
                for challenge in "${!pov_success_Java_full[@]}"; do echo "  - $challenge"; done
                ;;
            "Java&Delta")
                for challenge in "${!pov_success_Java_delta[@]}"; do echo "  - $challenge"; done
                ;;
            "BothLangs&Full")
                for challenge in "${!pov_success_Both_full[@]}"; do echo "  - $challenge"; done
                ;;
            "BothLangs&Delta")
                for challenge in "${!pov_success_Both_delta[@]}"; do echo "  - $challenge"; done
                ;;
            "C&Both")
                for challenge in "${!pov_success_C_Both[@]}"; do echo "  - $challenge"; done
                ;;
            "Java&Both")
                for challenge in "${!pov_success_Java_Both[@]}"; do echo "  - $challenge"; done
                ;;
            "BothLangs&Both")
                for challenge in "${!pov_success_Both_Both[@]}"; do echo "  - $challenge"; done
                ;;
        esac

        # List challenges for PATCH SUCCESS
        echo "Challenges for PATCH SUCCESS in $combination:"
        case "$combination" in
            "C&Full")
                for challenge in "${!patch_success_C_full[@]}"; do echo "  - $challenge"; done
                ;;
            "C&Delta")
                for challenge in "${!patch_success_C_delta[@]}"; do echo "  - $challenge"; done
                ;;
            "Java&Full")
                for challenge in "${!patch_success_Java_full[@]}"; do echo "  - $challenge"; done
                ;;
            "Java&Delta")
                for challenge in "${!patch_success_Java_delta[@]}"; do echo "  - $challenge"; done
                ;;
            "BothLangs&Full")
                for challenge in "${!patch_success_Both_full[@]}"; do echo "  - $challenge"; done
                ;;
            "BothLangs&Delta")
                for challenge in "${!patch_success_Both_delta[@]}"; do echo "  - $challenge"; done
                ;;
            "C&Both")
                for challenge in "${!patch_success_C_Both[@]}"; do echo "  - $challenge"; done
                ;;
            "Java&Both")
                for challenge in "${!patch_success_Java_Both[@]}"; do echo "  - $challenge"; done
                ;;
            "BothLangs&Both")
                for challenge in "${!patch_success_Both_Both[@]}"; do echo "  - $challenge"; done
                ;;
        esac
    done

    echo "========================"
    total_score=$((full_score + delta_score))
    echo "Total Score: $total_score"
    echo "========================"

    # Add results to CSV for this suffix
    # C-Mode: merge C&Full and C&Delta
    c_mode_score=$((combination_scores["C&Full"] + combination_scores["C&Delta"]))
    c_mode_pov=$(merge_challenge_arrays "pov_success_C_full" "pov_success_C_delta")
    c_mode_patch=$(merge_challenge_arrays "patch_success_C_full" "patch_success_C_delta")
    echo "C-Mode,$suffix,$c_mode_score,$c_mode_pov,$c_mode_patch" >> "$csv_file"

    # Java-Mode: merge Java&Full and Java&Delta
    java_mode_score=$((combination_scores["Java&Full"] + combination_scores["Java&Delta"]))
    java_mode_pov=$(merge_challenge_arrays "pov_success_Java_full" "pov_success_Java_delta")
    java_mode_patch=$(merge_challenge_arrays "patch_success_Java_full" "patch_success_Java_delta")
    echo "Java-Mode,$suffix,$java_mode_score,$java_mode_pov,$java_mode_patch" >> "$csv_file"

    # Delta-Mode: merge C&Delta and Java&Delta
    delta_mode_score=$((combination_scores["C&Delta"] + combination_scores["Java&Delta"]))
    delta_mode_pov=$(merge_challenge_arrays "pov_success_C_delta" "pov_success_Java_delta")
    delta_mode_patch=$(merge_challenge_arrays "patch_success_C_delta" "patch_success_Java_delta")
    echo "Delta-Mode,$suffix,$delta_mode_score,$delta_mode_pov,$delta_mode_patch" >> "$csv_file"

    # Full-Mode: merge C&Full and Java&Full
    full_mode_score=$((combination_scores["C&Full"] + combination_scores["Java&Full"]))
    full_mode_pov=$(merge_challenge_arrays "pov_success_C_full" "pov_success_Java_full")
    full_mode_patch=$(merge_challenge_arrays "patch_success_C_full" "patch_success_Java_full")
    echo "Full-Mode,$suffix,$full_mode_score,$full_mode_pov,$full_mode_patch" >> "$csv_file"

    # All: merge BothLangs&Delta and BothLangs&Full
    all_mode_score=$((combination_scores["BothLangs&Delta"] + combination_scores["BothLangs&Full"]))
    all_mode_pov=$(merge_challenge_arrays "pov_success_Both_delta" "pov_success_Both_full")
    all_mode_patch=$(merge_challenge_arrays "patch_success_Both_delta" "patch_success_Both_full")
    echo "All,$suffix,$all_mode_score,$all_mode_pov,$all_mode_patch" >> "$csv_file"

    echo "Results for $suffix added to CSV"
    echo ""
done

echo "========================"
echo "All results merged into: $csv_file"
echo "========================"