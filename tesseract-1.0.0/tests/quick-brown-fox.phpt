--TEST--
Recognize basic text
--FILE--
<?php

$t = new Operativeit\Tesseract("eng");
$t->setImage(__DIR__ . '/quick-brown-fox.png');
echo trim($t->getUTF8Text()) . "\n";
--EXPECT--
The quick brown fox
jumped over the lazy dog.
