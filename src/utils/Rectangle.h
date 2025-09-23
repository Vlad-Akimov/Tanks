#ifndef RECTANGLE_H
#define RECTANGLE_H

struct Rectangle {
    int x, y, width, height;
    bool intersects(const Rectangle& other) const;
    bool contains(int px, int py) const;
};

#endif