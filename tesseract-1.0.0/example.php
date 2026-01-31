<?php

// Check if extension is loaded
if (!extension_loaded('tesseract')) {
    echo "Extension 'tesseract' is not loaded.\n";
    echo "Try running with: php -d extension=modules/tesseract.so example.php\n";
    exit(1);
}

use Operativeit\Tesseract;

try {
    $tess = new Tesseract('eng'); // Optional language
    
    // Create a dummy image for testing if /tmp/foo.png doesn't exist
    $imagePath = __DIR__ . '/test_image.png';
    if (!file_exists($imagePath)) {
        // Create a simple text image using GD if available, or just warn
        if (function_exists('imagecreatetruecolor')) {
            $im = imagecreatetruecolor(200, 50);
            $bg = imagecolorallocate($im, 255, 255, 255);
            $text_color = imagecolorallocate($im, 0, 0, 0);
            imagefill($im, 0, 0, $bg);
            imagestring($im, 5, 10, 15,  "Hello Tesseract", $text_color);
            imagepng($im, $imagePath);
            imagedestroy($im);
        } else {
            die("Please provide a valid image path. Example: /tmp/foo.png\n");
        }
    }

    echo "Tesseract Version: " . Tesseract::VERSION . "\n";
    
    $text = $tess->setPageSegMode(Tesseract::PSM_AUTO)
                 ->setImage($imagePath)
                 ->getUTF8Text();

    echo "Extracted Text:\n";
    echo "---------------\n";
    echo $text . "\n";
    echo "---------------\n";

} catch (Exception $e) {
    echo "Error: " . $e->getMessage() . "\n";
}
