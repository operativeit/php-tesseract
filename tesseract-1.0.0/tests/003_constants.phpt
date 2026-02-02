--TEST--
Check Tesseract Constants
--SKIPIF--
<?php if (!extension_loaded("tesseract")) print "skip"; ?>
--FILE--
<?php
echo defined("Operativeit\Tesseract::OEM_DEFAULT") ? "OEM_DEFAULT OK\n" : "Fail\n";
echo defined("Operativeit\Tesseract::PSM_AUTO") ? "PSM_AUTO OK\n" : "Fail\n";
?>
--EXPECTREGEX--
OEM_DEFAULT OK\n?.*
PSM_AUTO OK\n?.*
