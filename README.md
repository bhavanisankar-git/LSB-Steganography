# 🔒 LSB Steganography in C  

This project demonstrates **Least Significant Bit (LSB) Steganography** using the C programming language.  
It allows you to **embed secret text into an image** and later **extract it back**, without noticeable changes to the image.  

## 📌 Features  
- Encode secret text into 24-bit BMP images  
- Decode hidden text from stego images  
- Minimal impact on image quality  
- Command-line interface with modular C code  

## 📂 Project Structure  
```
steganography/

│   ├── main.c            # Entry point
│   ├── encode.c / encode.h  # Encoding functions
│   ├── decode.c / decode.h  # Decoding functions
│   ├── common.h          # Shared definitions
│   ├── types.h           # Custom data types
│   ├── makefile          # Build instructions
│   ├── beautiful.bmp     # Sample cover image
│   ├── stego.bmp         # Generated stego image
│   ├── secret.txt        # Secret message to embed
│   └── output.txt        # Extracted secret message
```

## 🛠️ Compilation & Execution  

1. Navigate to the code directory:  
   ```bash
   cd -SkeletonCode
   ```

2. Compile using the provided makefile:  
   ```bash
   make
   ```

   This will generate an executable (e.g., `stego` or `a.out`).  

3. Run the program:  

   **Encoding:**  
   ```bash
   ./stego encode beautiful.bmp secret.txt stego.bmp
   ```  

   **Decoding:**  
   ```bash
   ./stego decode stego.bmp output.txt
   ```  

## 🚀 How LSB Works  
- Each pixel in a 24-bit BMP image is represented by 3 bytes (R, G, B).  
- The **least significant bit** of each channel is replaced with one bit of the secret message.  
- Example:  
  - Original pixel (Blue): `10110010` (178)  
  - After embedding: `10110011` (179)  
  - Difference is imperceptible to the human eye.  

## 🔮 Future Improvements  
- Support for other image formats (PNG, JPEG)  
- Encrypt message before embedding  
- GUI-based tool for non-technical users  

## 📜 License  
This project is for educational purposes. You are free to use and modify it.  


HAPPY HIDING :)
