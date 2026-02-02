--TEST--
Set Region of Interest
--SKIPIF--
<?php if (!extension_loaded("tesseract")) print "skip"; ?>
--FILE--
<?php
$t = new Operativeit\Tesseract("eng");
// Just verify it doesn't crash
$t->setRectangle(10, 10, 100, 100);
echo "ROI Set";
?>
--EXPECTREGEX--
ROI Set.*
