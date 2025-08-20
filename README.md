# Seam Carving Image Resizer

This project implements **Seam Carving** for content-aware image resizing using **C++** and **OpenCV**. It allows you to reduce the width and height of an image while preserving important content.

---

## Technologies Used

- **C++**
- **OpenCV** (for image handling and display)
- Standard C++ libraries (`iostream`, `string`, `cstring`)

---

## Features

- Reduce image width (vertical seam removal)
- Reduce image height (horizontal seam removal)
- Displays image dynamically with seams marked in red
- Saves the resized image with prefix `Compressed_`

---

## Compilation

Make sure you have **OpenCV** installed on your system.

g++ main.cpp -o seam_carving \`pkg-config --cflags --libs opencv4\`


---

## Running the Program

./seam_carving <image_path>

### Example:

./seam_carving sample.jpg

1. The program will show the original image size.
2. Enter the **desired width**.
3. Enter the **desired height**.
4. The image will be displayed dynamically as seams are removed.
5. The final image will be saved in the current directory with the name:

Compressed_<original_filename>

---

## Notes

- Only works with images that OpenCV can read (e.g., JPG, PNG).
- Ensure the desired dimensions are smaller than the original image.
- The program uses **dynamic programming** for seam calculation and **dual gradient energy** for better results.

---

## Output

If your input image is `sample.jpg` and the desired size is smaller, the program will save:

Compressed_sample.jpg

and display it in a window until a key is pressed.

---

## Author

Made by **[Param Modi](https://github.com/iParamModi)**  
MTech CSE student at **IIIT Hyderabad**  
This was **Question 1 of Assignment 1 for DSA**.

---

## License

This project is for educational purposes.
