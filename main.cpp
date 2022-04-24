#include <opencv2/opencv.hpp>

#define COM_C 0
#define COM_S 1
#define COM_CONVERT 2
#define COM_N 3
#define COM_G 4
#define COM_Q 5
#define COM_E 6
#define COM_Z 7
#define COM_X 8
#define COM_A 9
#define COM_B 10
#define COM_P 11

#define NUM_COM 12

using namespace cv;

void print_commands()
{
    printf("Comandos:\n");
    printf("C : Canny\n");
    printf("S : Sobel\n");
    printf("H : + Contrast\n");
    printf("J : - Contrast\n");
    printf("K : + Brightness\n");
    printf("L : - Brightness\n");
    printf("N : Negative\n");
    printf("G : Grayscale\n");
    printf("A : Resize 1/2\n");
    printf("Q : Rotate Counterclockwise\n");
    printf("E : Rotate Clockwise\n");
    printf("Z : Horizontal Mirror\n");
    printf("X : Vertical Mirror\n");
    printf("\nR : Reset\n");
    printf("\nP : Start/Stop Recording\n");
    printf("\nESC : Exit");
}

int main (int argc,char **argv)
{
    int camera = 0;
    VideoCapture cap;
    VideoWriter video;
    
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if (!cap.open(camera)) {
        return 0;
    }
    char keyPressed = 0;

    namedWindow("Original Video", WINDOW_AUTOSIZE);
    namedWindow("Video Effects", WINDOW_AUTOSIZE);
    int blur = 0;
    double alpha = 1;
    double beta = 0;
    int ini_trackbar = 0;
    int commands[NUM_COM] = { 0 };

    createTrackbar("Blur", "Video Effects", &blur, 30);
    
    int codec;
    for (;;)
    {
        system("cls");
        print_commands();
        Mat src;
        Mat dst;
        Mat aux;
        cap >> src;
        cap >> dst;
        if (src.empty()) {
            break; // end of video stream
        }


        if (blur) {
            GaussianBlur(dst, dst, Size(blur | 1, blur | 1), 3, 0);
        }
        for (int i = 0; i < NUM_COM; i++) {
            for (int j = 0; j < commands[i]; j++) {
                switch (i) {
                case COM_C: // Canny
                    aux = dst.clone();
                    Canny(aux, dst, 50, 50);
                    cvtColor(dst, dst, COLOR_GRAY2BGR); // needed for video capture
                    break;
                case COM_S: // Sobel
                    Sobel(dst, dst, CV_8UC1, 0, 1, 3, 1, 1, BORDER_DEFAULT);
                    break;
                case COM_CONVERT:
                    dst.convertTo(dst, -1, alpha, beta);
                    break;
                case COM_G: // grayscale
                    cvtColor(dst, dst, COLOR_BGR2GRAY);
                    cvtColor(dst, dst, COLOR_GRAY2BGR); // needed for video capture
                    break;
                case COM_Q: // rotate CCW
                    rotate(dst, dst, ROTATE_90_COUNTERCLOCKWISE);
                    break;
                case COM_E: // rotate CW
                    rotate(dst, dst, ROTATE_90_CLOCKWISE);
                    break;
                case COM_Z:// hMirror
                    flip(dst, dst, 1);
                    break;
                case COM_X: // vMirror
                    flip(dst, dst, 0);
                    break;
                case COM_A: // Resize
                    resize(dst, dst, Size(), 0.5, 0.5);
                    break;
                }
            }
        }
        imshow("Original Video", src);
        imshow("Video Effects", dst);

        if (commands[COM_P]) {
            video.write(dst);
        }
        keyPressed = waitKey(1);
        switch (keyPressed) {
        case 'c': // Canny
        case 'C':
            commands[COM_C]++;
            if (commands[COM_C] == 2) {
                commands[COM_C] = 0;
            }
            break;
        case 's': // Sobel
        case 'S':
            commands[COM_S]++;
            if (commands[COM_S] == 2) {
                commands[COM_S] = 0;
            }
            break;
        case 'g': // grayscale
        case 'G':
            commands[COM_G]++;
            if (commands[COM_G] == 2) {
                commands[COM_G] = 0;
            }
            break;
        case 'h': // +contrast
        case 'H':
            alpha *= 2;
            commands[COM_CONVERT] = 1;
            break;
        case 'j': // -contrast
        case 'J':
            alpha /= 2;
            commands[COM_CONVERT] = 1;
            break;
        case 'k': // +brightness
        case 'K':
            beta += 10;
            commands[COM_CONVERT] = 1;
            break;
        case 'l': // -brightness
        case 'L':
            beta -= 10;
            commands[COM_CONVERT] = 1;
            break;
        case 'n': // negative
        case 'N':
            alpha *= -1;
            beta = 255 - beta;
            commands[COM_CONVERT] = 1;
            break;
        case 'q': // rotate CCW
        case 'Q': 
            commands[COM_Q]++;
            break;
        case 'e': // rotate CW
        case 'E': 
            commands[COM_E]++;
            break;
        case 'z': // hMirror
        case 'Z':
            commands[COM_Z]++;
            break;
        case 'x': // vMirror
        case 'X':
            commands[COM_X]++;
            break;
        case 'a': // Resize
        case 'A':
            commands[COM_A]++;
            break;
        case 'p': // record
        case 'P':
            commands[COM_P]++;
            if (commands[COM_P] == 2) {
                commands[COM_P] = 0;
                video.release();
            } else {
                codec = video.fourcc('M', 'J', 'P', 'G');
                video.open("output.avi", codec, 10, dst.size());
            }
            break;
        case 'r': // reset
        case 'R':
            for (int i = 0; i < NUM_COM-1; i++) {
                commands[i] = 0;
            }
            alpha = 1;
            beta = 0;
            break;
        }

        if (keyPressed == 27)
            break; // stop capturing by pressing ESC
    }
    cap.release(); // release the VideoCapture object
    return 0;
}