#include <iostream>

#include "Graphics/Window.h"
#include "Graphics/Gui/Gui.h"
#include "Editor/Editor.h"

using namespace std;


//int **submatrix2(int **matrix, unsigned int n, unsigned int x, unsigned int y) {
////    int **submatrix = new int *[n - 1];
//    int **submatrix = (int**) malloc(sizeof(int*) * (n - 1));
//    int subi = 0;
//    for (int i = 0; i < n; i++) {
////        submatrix[subi] = new int[n - 1];
//        submatrix[subi] = (int*) malloc(sizeof(int) * (n - 1));
//        int subj = 0;
//        if (i == y) {
//            continue;
//        }
//        for (int j = 0; j < n; j++) {
//            if (j == x) {
//                continue;
//            }
//            submatrix[subi][subj] = matrix[i][j];
//            subj++;
//        }
//        subi++;
//    }
//    return submatrix;
//}

//int determinant2(int **matrix, unsigned int n, bool deleteMatrix = false) {
//    int det = 0;
//    if (n == 2) {
//        return matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
//    }
//    for (int x = 0; x < n; ++x) {
//        det += ((x % 2 == 0 ? 1 : -1) * matrix[0][x] * determinant2(submatrix2(matrix, n, x, 0), n - 1, true));
//    }

//    if (deleteMatrix)
//    {
//        for (int i = 0; i < n; i++)
//            free(matrix[i]);
//        free(matrix);
//    }

//    return det;
//}


//int **submatrix1(int **matrix, unsigned int n, unsigned int x, unsigned int y) {
//    int **submatrix = new int *[n - 1];
////    int **submatrix = (int**) malloc(sizeof(int*) * (n - 1));
//    int subi = 0;
//    for (int i = 0; i < n; i++) {
//        submatrix[subi] = new int[n - 1];
////        submatrix[subi] = (int*) malloc(sizeof(int) * (n - 1));
//        int subj = 0;
//        if (i == y) {
//            continue;
//        }
//        for (int j = 0; j < n; j++) {
//            if (j == x) {
//                continue;
//            }
//            submatrix[subi][subj] = matrix[i][j];
//            subj++;
//        }
//        subi++;
//    }
//    return submatrix;
//}

//int determinant1(int **matrix, unsigned int n, bool deleteMatrix = false) {
//    int det = 0;
//    if (n == 2)
//    {
//        return matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
//    }
//    for (int x = 0; x < n; ++x)
//    {
//        det += ((x % 2 == 0 ? 1 : -1) * matrix[0][x] * determinant1(submatrix1(matrix, n, x, 0), n - 1, true));
//    }

//    if (deleteMatrix)
//    {
//        for (int i = 0; i < n; i++)
//            delete matrix[i];
//        delete matrix;
//    }

//    return det;
//}


int main(int argc, char **argv)
{
    Window* window = Window::Create();
    if (!window)
    {
        cout << "Window creation error\n";
        return -1;
    }
    OpenclSystem::Get().Init();

    Editor* editor = new Editor;

    window->AddItem(editor);

    window->Open();

    delete editor;

    Window::Destroy();
    OpenclSystem::Get().Destroy();
    return 0;
}
