--TEST--
Set Page Segmentation Mode
--SKIPIF--
<?php if (!extension_loaded("tesseract")) print "skip"; ?>
--FILE--
<?php
$t = new Operativeit\Tesseract("eng");
$t->setPageSegMode(Operativeit\Tesseract::PSM_SINGLE_WORD);
echo "PSM Set";
?>
--EXPECTREGEX--
PSM Set.*
