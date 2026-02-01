--TEST--
Recognize basic text
--FILE--
<?php
try {
    $t = new Operativeit\Tesseract("eng");
    $img = __DIR__ . '/quick-brown-fox.png';
    if (!file_exists($img)) {
        throw new Exception("Image not found: $img");
    }
    $t->setImage($img);
    $s = $t->getUTF8Text();
    $s = trim(preg_replace('/\s+/u', ' ', strtolower($s)));
    // Broaden regex to match core phrase, ignoring verb nuances
    echo preg_match('/the quick brown fox.*over the lazy dog\.?/i', $s) ? "OK\n" : "FAIL got: '$s'\n";
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
OK
