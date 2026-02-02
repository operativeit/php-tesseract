--TEST--
Check Tesseract Version
--SKIPIF--
<?php if (!extension_loaded("tesseract")) print "skip"; ?>
--FILE--
<?php
echo defined("Operativeit\Tesseract::VERSION") ? "Version Defined" : "No Version";
?>
--EXPECTREGEX--
Version Defined.*
