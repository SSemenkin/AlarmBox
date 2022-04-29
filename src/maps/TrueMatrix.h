#ifndef TRUEMATRIX_H
#define TRUEMATRIX_H

#include <QImage>
#include <QPixmap>
#include <QDebug>

class TrueMatrix{
public:
    explicit TrueMatrix(const QImage &cellImage, int cellIdentity=1)
        : __size(cellImage.size()) {
        int height = __size.height();
        int width = __size.width();
        __matrix = new short int* [height];
        for(short int iter = 0; iter < height; iter++){
            __matrix[iter] = new short int[width]{};
        }
        addCellImage(cellImage, cellIdentity);
    };
    explicit TrueMatrix(const QPixmap &cellImage, int cellIdentity=1) : TrueMatrix(cellImage.toImage(), cellIdentity) {
    }


    ~TrueMatrix(){
        for(short int iter = 0; iter < __size.height(); iter++){
            delete[] __matrix[iter];
        }
        delete[] __matrix;
        __matrix = nullptr;
    }

    QSize size(){
        return __size;
    };

    int shoot(int x, int y){
        if(x > __size.width() || x < 0 || y > __size.height() || y < 0){
            return -1;
        } else {
            return __matrix[x][y] - 1;
        }
    }

    int shoot(QPointF point){
        return shoot(point.x(), point.y());
    }

    void addCellImage(const QPixmap &cellImage, int cellIdentity = 2){
        QImage image = cellImage.toImage();
        addCellImage(image, cellIdentity);
    }

    void addCellImage(const QImage &cellImage, int cellIdentity = 2){
        cellIdentity++;
        QSize size = cellImage.size();
        int height = __size.height();
        int width = __size.width();
        if(__size == size){
            for(int row = 0; row < height; row++){
                for(int field = 0; field < width; field++){
                    if(cellImage.pixelColor(row, field) == QColor(0, 0, 0, 0)){

                    } else {
                        __matrix[row][field] = cellIdentity;
                    }
                }
            }

        } else {
            qDebug() << "Error size of cell image";
        }
    }

private:
    short int **__matrix;
    QSize __size;
};

#endif // TRUEMATRIX_H
