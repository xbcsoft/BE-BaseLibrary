<?php

// --- Utility function for streaming ---
function initialize_streaming($max_time = 30)
{
	set_time_limit($max_time);
	@ob_end_clean(); // Clean any existing output buffers
	header('Content-Type: text/plain');
	header('X-Accel-Buffering: no'); // Disable nginx buffering
	ob_implicit_flush(); // Flush output automatically
}

// --- Main search logic ---
$q = $_GET['q'] ?? '';
$q = trim($q);

if ($q === '') {
	exit; // Exit if no query
}

initialize_streaming();

$packFile = __DIR__ . '/collection.pack';
if (!file_exists($packFile)) {
	echo "Error: collection.pack not found.\0";
	exit;
}

$data = file_get_contents($packFile);
$pos = 0;

// Read file count
if (strlen($data) < 4) exit;
$count = unpack('V', substr($data, $pos, 4))[1];
$pos += 4;

// Read index table
$index = [];
for ($i = 0; $i < $count; $i++) {
	if (strlen($data) < $pos + 4) break;
	$nameLen = unpack('V', substr($data, $pos, 4))[1];
	$pos += 4;

	if (strlen($data) < $pos + $nameLen) break;
	$name = substr($data, $pos, $nameLen);
	$pos += $nameLen;

	if (strlen($data) < $pos + 8) break;
	$start = unpack('V', substr($data, $pos, 4))[1];
	$pos += 4;
	$len = unpack('V', substr($data, $pos, 4))[1];
	$pos += 4;

	$index[] = ['name' => $name, 'start' => $start, 'len' => $len];
}

// Main data body
$body = substr($data, $pos);

// Search and stream results
foreach ($index as $info) {
	$content = substr($body, $info['start'], $info['len']);
	if (strpos($content, $q) !== false) {
		// Send the matched filename followed by a null character delimiter
		echo json_encode(['name' => $info['name']]) . "\0";
	}
}
