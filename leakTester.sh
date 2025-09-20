#!/bin/bash

# Simple Shell Leak Testing Script
# Tests for memory leaks and file descriptor leaks

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Shell executable name
SHELL_EXEC="./hsh"

# Test results counters
TESTS_RUN=0
TESTS_PASSED=0
MEMORY_LEAKS=0
FD_LEAKS=0

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}    Simple Shell Leak Testing Script   ${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
}

print_test() {
    echo -e "${YELLOW}[TEST $1]${NC} $2"
}

print_pass() {
    echo -e "${GREEN}✓ PASS:${NC} $1"
    ((TESTS_PASSED++))
}

print_fail() {
    echo -e "${RED}✗ FAIL:${NC} $1"
}

print_leak() {
    echo -e "${RED}⚠ LEAK DETECTED:${NC} $1"
}

# Check if shell executable exists
check_shell_exists() {
    if [ ! -f "$SHELL_EXEC" ]; then
        echo -e "${RED}Error: Shell executable '$SHELL_EXEC' not found!${NC}"
        echo "Please compile your shell first:"
        echo "gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh"
        exit 1
    fi
}

# Check if required tools are installed
check_tools() {
    if ! command -v valgrind &> /dev/null; then
        echo -e "${RED}Error: valgrind is not installed!${NC}"
        echo "Install with: sudo apt-get install valgrind"
        exit 1
    fi
    
    if ! command -v lsof &> /dev/null; then
        echo -e "${RED}Error: lsof is not installed!${NC}"
        echo "Install with: sudo apt-get install lsof"
        exit 1
    fi
}

# Test memory leaks with valgrind
test_memory_leaks() {
    print_test $((++TESTS_RUN)) "Memory leak test - Basic commands"
    
    # Create test input
    echo -e "/bin/echo Hello\n/bin/pwd\nexit" > /tmp/shell_test_input
    
    # Run valgrind
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
             --error-exitcode=1 --log-file=/tmp/valgrind_output \
             $SHELL_EXEC < /tmp/shell_test_input > /dev/null 2>&1
    
    # Check valgrind output
    if grep -q "All heap blocks were freed" /tmp/valgrind_output && \
       grep -q "no leaks are possible" /tmp/valgrind_output; then
        print_pass "No memory leaks detected"
    else
        print_leak "Memory leaks found"
        ((MEMORY_LEAKS++))
        echo "Valgrind output:"
        cat /tmp/valgrind_output | grep -E "(LEAK|lost|reachable)"
    fi
    
    # Cleanup
    rm -f /tmp/shell_test_input /tmp/valgrind_output
}

# Test memory leaks with built-in commands
test_builtin_memory_leaks() {
    print_test $((++TESTS_RUN)) "Memory leak test - Built-in commands"
    
    # Create test input with built-ins
    echo -e "env\nexit" > /tmp/shell_builtin_test
    
    # Run valgrind
    valgrind --leak-check=full --show-leak-kinds=all \
             --error-exitcode=1 --log-file=/tmp/valgrind_builtin \
             $SHELL_EXEC < /tmp/shell_builtin_test > /dev/null 2>&1
    
    # Check valgrind output
    if grep -q "All heap blocks were freed" /tmp/valgrind_builtin && \
       grep -q "no leaks are possible" /tmp/valgrind_builtin; then
        print_pass "No memory leaks in built-ins"
    else
        print_leak "Memory leaks in built-in commands"
        ((MEMORY_LEAKS++))
        echo "Valgrind output:"
        cat /tmp/valgrind_builtin | grep -E "(LEAK|lost|reachable)"
    fi
    
    # Cleanup
    rm -f /tmp/shell_builtin_test /tmp/valgrind_builtin
}

# Test memory leaks with invalid commands
test_invalid_command_memory() {
    print_test $((++TESTS_RUN)) "Memory leak test - Invalid commands"
    
    # Create test input with invalid commands
    echo -e "nonexistent_command\n/invalid/path\nexit" > /tmp/shell_invalid_test
    
    # Run valgrind
    valgrind --leak-check=full --show-leak-kinds=all \
             --error-exitcode=1 --log-file=/tmp/valgrind_invalid \
             $SHELL_EXEC < /tmp/shell_invalid_test > /dev/null 2>&1
    
    # Check valgrind output
    if grep -q "All heap blocks were freed" /tmp/valgrind_invalid && \
       grep -q "no leaks are possible" /tmp/valgrind_invalid; then
        print_pass "No memory leaks with invalid commands"
    else
        print_leak "Memory leaks with invalid commands"
        ((MEMORY_LEAKS++))
        echo "Valgrind output:"
        cat /tmp/valgrind_invalid | grep -E "(LEAK|lost|reachable)"
    fi
    
    # Cleanup
    rm -f /tmp/shell_invalid_test /tmp/valgrind_invalid
}

# Test file descriptor leaks
test_fd_leaks() {
    print_test $((++TESTS_RUN)) "File descriptor leak test"
    
    # Get initial FD count
    echo -e "/bin/echo Starting FD test\nexit" > /tmp/shell_fd_test
    
    # Run shell and check FDs
    $SHELL_EXEC < /tmp/shell_fd_test > /dev/null 2>&1 &
    SHELL_PID=$!
    
    sleep 1
    
    # Check if process is still running (it shouldn't be)
    if kill -0 $SHELL_PID 2>/dev/null; then
        FD_COUNT=$(lsof -p $SHELL_PID 2>/dev/null | wc -l)
        if [ $FD_COUNT -gt 10 ]; then
            print_leak "Possible file descriptor leak ($FD_COUNT FDs open)"
            ((FD_LEAKS++))
            echo "Open file descriptors:"
            lsof -p $SHELL_PID 2>/dev/null
        else
            print_pass "No excessive file descriptors"
        fi
        kill $SHELL_PID 2>/dev/null
    else
        print_pass "Shell exited properly"
    fi
    
    # Cleanup
    rm -f /tmp/shell_fd_test
}

# Test for memory leaks with arguments
test_argument_memory_leaks() {
    print_test $((++TESTS_RUN)) "Memory leak test - Commands with arguments"
    
    # Create test input with various arguments
    echo -e "/bin/ls -la\n/bin/echo Hello World Test\n/bin/cat /etc/passwd\nexit" > /tmp/shell_args_test
    
    # Run valgrind
    valgrind --leak-check=full --show-leak-kinds=all \
             --error-exitcode=1 --log-file=/tmp/valgrind_args \
             $SHELL_EXEC < /tmp/shell_args_test > /dev/null 2>&1
    
    # Check valgrind output
    if grep -q "All heap blocks were freed" /tmp/valgrind_args && \
       grep -q "no leaks are possible" /tmp/valgrind_args; then
        print_pass "No memory leaks with arguments"
    else
        print_leak "Memory leaks with command arguments"
        ((MEMORY_LEAKS++))
        echo "Valgrind output:"
        cat /tmp/valgrind_args | grep -E "(LEAK|lost|reachable)"
    fi
    
    # Cleanup
    rm -f /tmp/shell_args_test /tmp/valgrind_args
}

# Test PATH resolution memory leaks
test_path_memory_leaks() {
    print_test $((++TESTS_RUN)) "Memory leak test - PATH resolution"
    
    # Create test input with PATH commands
    echo -e "ls\necho Hello\npwd\nexit" > /tmp/shell_path_test
    
    # Run valgrind
    valgrind --leak-check=full --show-leak-kinds=all \
             --error-exitcode=1 --log-file=/tmp/valgrind_path \
             $SHELL_EXEC < /tmp/shell_path_test > /dev/null 2>&1
    
    # Check valgrind output
    if grep -q "All heap blocks were freed" /tmp/valgrind_path && \
       grep -q "no leaks are possible" /tmp/valgrind_path; then
        print_pass "No memory leaks in PATH resolution"
    else
        print_leak "Memory leaks in PATH resolution"
        ((MEMORY_LEAKS++))
        echo "Valgrind output:"
        cat /tmp/valgrind_path | grep -E "(LEAK|lost|reachable)"
    fi
    
    # Cleanup
    rm -f /tmp/shell_path_test /tmp/valgrind_path
}

# Test EOF handling memory leaks
test_eof_memory_leaks() {
    print_test $((++TESTS_RUN)) "Memory leak test - EOF handling"
    
    # Create test that sends EOF
    echo -e "/bin/echo Testing EOF" > /tmp/shell_eof_test
    
    # Run valgrind (EOF will terminate shell)
    valgrind --leak-check=full --show-leak-kinds=all \
             --error-exitcode=1 --log-file=/tmp/valgrind_eof \
             $SHELL_EXEC < /tmp/shell_eof_test > /dev/null 2>&1
    
    # Check valgrind output
    if grep -q "All heap blocks were freed" /tmp/valgrind_eof && \
       grep -q "no leaks are possible" /tmp/valgrind_eof; then
        print_pass "No memory leaks on EOF"
    else
        print_leak "Memory leaks on EOF handling"
        ((MEMORY_LEAKS++))
        echo "Valgrind output:"
        cat /tmp/valgrind_eof | grep -E "(LEAK|lost|reachable)"
    fi
    
    # Cleanup
    rm -f /tmp/shell_eof_test /tmp/valgrind_eof
}

# Print final results
print_summary() {
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}           TEST SUMMARY                 ${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo -e "Total tests run: ${TESTS_RUN}"
    echo -e "Tests passed: ${GREEN}${TESTS_PASSED}${NC}"
    echo -e "Tests failed: ${RED}$((TESTS_RUN - TESTS_PASSED))${NC}"
    echo -e "Memory leaks detected: ${RED}${MEMORY_LEAKS}${NC}"
    echo -e "FD leaks detected: ${RED}${FD_LEAKS}${NC}"
    echo ""
    
    if [ $MEMORY_LEAKS -eq 0 ] && [ $FD_LEAKS -eq 0 ]; then
        echo -e "${GREEN}🎉 CONGRATULATIONS! No leaks detected!${NC}"
        echo -e "${GREEN}Your shell appears to be leak-free.${NC}"
    else
        echo -e "${RED}⚠️  ATTENTION NEEDED!${NC}"
        echo -e "${RED}Please fix the detected leaks before submission.${NC}"
    fi
    echo ""
}

# Main execution
main() {
    print_header
    check_shell_exists
    check_tools
    
    echo -e "${YELLOW}Running leak tests...${NC}"
    echo ""
    
    test_memory_leaks
    test_builtin_memory_leaks
    test_invalid_command_memory
    test_argument_memory_leaks
    test_path_memory_leaks
    test_eof_memory_leaks
    test_fd_leaks
    
    print_summary
}

# Run the tests
main "$@"