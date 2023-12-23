#include "stdio.h"

#include "linalg.h"
#include "console.h"

int main() {
    console::console console{119, 29};
    console.drawString(5, 5, "hello world");
    for(int i = 1; i < 2000; ++i) {
        console.drawInt(10, 10, i);
        console.repaint();
    }
}