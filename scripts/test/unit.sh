# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../.."

echo "[Tests] Compiling and running unit tests..."

FAILED=0
TOTAL=0

# Compile and run each .cpp file in unit-tests/
for test_file in unit-tests/*.cpp; do
    test_name=$(basename "$test_file" .cpp)
    echo "[Tests] Running $test_name..."
    
    # Compile
    # Note: We link Url.cpp as it's a common dependency for these tests
    g++ -Isrc "$test_file" src/url/Url.cpp -o "unit-tests/$test_name.bin"
    
    if [ $? -ne 0 ]; then
        echo "[Tests] $test_name: FAILED TO COMPILE"
        FAILED=$((FAILED + 1))
        TOTAL=$((TOTAL + 1))
        continue
    fi
    
    # Run
    "./unit-tests/$test_name.bin"
    if [ $? -ne 0 ]; then
        FAILED=$((FAILED + 1))
    fi
    TOTAL=$((TOTAL + 1))
    
    # Clean up binary
    rm "unit-tests/$test_name.bin"
    echo "--------------------------------------"
done

if [ $FAILED -eq 0 ]; then
    echo "[Tests] ALL $TOTAL TESTS PASSED!"
    exit 0
else
    echo "[Tests] $FAILED/$TOTAL TESTS FAILED!"
    exit 1
fi
