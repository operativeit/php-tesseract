--TEST--
Recognize basic text
--FILE--
<?php
try {
    $t = new Operativeit\Tesseract("eng");
    $t->setImage(__DIR__ . '/quick-brown-fox.png');
    $s = $t->getUTF8Text();
    $s = trim(preg_replace('/\s+/u', ' ', strtolower($s)));
    echo preg_match('/the quick brown fox.*jump(s|ed) over the lazy dog\.?/i', $s) ? "OK\n" : "FAIL got: '$s'\n";
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
OK
