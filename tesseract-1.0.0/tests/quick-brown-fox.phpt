--TEST--
Recognize basic text
--FILE--
<?php
$t = new Operativeit\Tesseract("eng");
$t->setImage(__DIR__ . '/quick-brown-fox.png');
$s = $t->getUTF8Text();
$s = trim(preg_replace('/\s+/u', ' ', strtolower($s)));
echo preg_match('/the quick brown fox.*jumped over the lazy dog\.?/i', $s) ? "OK\n" : "FAIL\n$s\n";
?>
--EXPECT--
OK
