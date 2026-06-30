package main

import (
	"bufio"
	"crypto/md5"
	"encoding/hex"
	"os"
	"strings"
)

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		line := scanner.Text()
		line = strings.TrimRight(line, "\r\n")
		hash := md5.Sum([]byte(line))
		os.Stdout.WriteString(hex.EncodeToString(hash[:]) + "\n")
	}
}
