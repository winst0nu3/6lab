#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

// Структура для матрицы смежности
typedef struct {
    int vertices;
    int** matrix;
} Graph;

// Создание графа
Graph* create_graph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->vertices = vertices;
    graph->matrix = (int**)malloc(vertices * sizeof(int*));
    for (int i = 0; i < vertices; i++) {
        graph->matrix[i] = (int*)calloc(vertices, sizeof(int));
    }
    return graph;
}

// Освобождение памяти графа
void free_graph(Graph* graph) {
    for (int i = 0; i < graph->vertices; i++) {
        free(graph->matrix[i]);
    }
    free(graph->matrix);
    free(graph);
}

// Генерация случайного неориентированного графа с петлями
void generate_random_graph(Graph* graph) {
    for (int i = 0; i < graph->vertices; i++) {
        // Петля 
        graph->matrix[i][i] = rand() % 2;

        // Обычные рёбра 
        for (int j = i + 1; j < graph->vertices; j++) {
            int random_value = rand() % 2;
            graph->matrix[i][j] = random_value;
            graph->matrix[j][i] = random_value;
        }
    }
}

// Вывод матрицы смежности
void print_matrix(Graph* graph) {
    printf("Матрица смежности (%d вершин):\n", graph->vertices);
    printf("   ");
    for (int i = 0; i < graph->vertices; i++) {
        printf("%2d ", i);
    }
    printf("\n");

    for (int i = 0; i < graph->vertices; i++) {
        printf("%2d: ", i);
        for (int j = 0; j < graph->vertices; j++) {
            printf("%2d ", graph->matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Функция для ввода вершин с проверкой и повторным вводом
void input_vertices_with_retry(Graph* graph, int* u, int* v, const char* operation_name) {
    int valid_input = 0;
    while (!valid_input) {
        printf("Введите две вершины для %s (через пробел): ", operation_name);
        if (scanf_s("%d %d", u, v) != 2) {
            printf("Ошибка ввода, введите два числа.\n");
            while (getchar() != '\n'); // Очистка буфера ввода
            continue;
        }

        if (*u >= graph->vertices || *v >= graph->vertices || *u < 0 || *v < 0) {
            printf("Ошибка: вершины должны быть в диапазоне от 0 до %d\n", graph->vertices - 1);
            continue;
        }

        if (*u == *v) {
            printf("Ошибка: вершины не должны совпадать\n");
            continue;
        }

        valid_input = 1;
    }
}

// Функция для ввода вершины с проверкой и повторным вводом
void input_vertex_with_retry(Graph* graph, int* w, const char* operation_name) {
    int valid_input = 0;
    while (!valid_input) {
        printf("Введите вершину для %s: ", operation_name);
        if (scanf_s("%d", w) != 1) {
            printf("Ошибка ввода, введите число.\n");
            while (getchar() != '\n'); // Очистка буфера ввода
            continue;
        }

        if (*w >= graph->vertices || *w < 0) {
            printf("Ошибка: вершина должна быть в диапазоне от 0 до %d\n", graph->vertices - 1);
            continue;
        }

        valid_input = 1;
    }
}

// Отождествление вершин 
Graph* identify_vertices_matrix(Graph* graph, int v, int u) {
    // Создаем новый граф с vertices-1 вершиной
    int new_vertices = graph->vertices - 1;
    Graph* new_graph = create_graph(new_vertices);

    // Определяем, какую вершину сохраняем и какую удаляем
    int keep_vertex = (v < u) ? v : u;
    int remove_vertex = (v < u) ? u : v;

    printf("Отождествление вершин %d и %d:\n", u, v);
    printf("Сохраняем вершину %d, удаляем вершину %d\n", keep_vertex, remove_vertex);

    // Заполняем новый граф
    for (int i = 0, new_i = 0; i < graph->vertices; i++) {
        if (i == remove_vertex) continue; // Пропускаем удаляемую вершину

        for (int j = 0, new_j = 0; j < graph->vertices; j++) {
            if (j == remove_vertex) continue; // Пропускаем удаляемую вершину

            int value;
            if (i == keep_vertex) {
                // Для сохраняемой вершины: объединяем связи с текущей вершиной j
                value = graph->matrix[keep_vertex][j] || graph->matrix[remove_vertex][j];
            }
            else if (j == keep_vertex) {
                // Для связей с сохраняемой вершиной: объединяем связи из текущей вершины i
                value = graph->matrix[i][keep_vertex] || graph->matrix[i][remove_vertex];
            }
            else {
                // Для остальных вершин: просто копируем
                value = graph->matrix[i][j];
            }

            new_graph->matrix[new_i][new_j] = value;
            new_j++;
        }
        new_i++;
    }

    return new_graph;
}

// Стягивание ребра (матричное)
Graph* contract_edge_matrix(Graph* graph, int v, int u) {
    if (u == v) {
        printf("Ошибка: нельзя стягивать петлю\n");
        return NULL;
    }

    // Проверяем смежность вершин
    if (graph->matrix[v][u] == 0) {
        printf("Ошибка: вершины %d и %d не соединены ребром\n", v, u);
        return NULL;
    }

    printf("Стягивание ребра между вершинами %d и %d:\n", v, u);

    // Создаем новый граф с vertices-1 вершиной
    int new_vertices = graph->vertices - 1;
    Graph* new_graph = create_graph(new_vertices);

    // Определяем, какую вершину сохраняем и какую удаляем
    int keep_vertex = (v < u) ? v : u;
    int remove_vertex = (v < u) ? u : v;

    // Заполняем новый граф (как в отождествлении, НО убираем петлю от стягиваемого ребра)
    for (int i = 0, new_i = 0; i < graph->vertices; i++) {
        if (i == remove_vertex) continue;

        for (int j = 0, new_j = 0; j < graph->vertices; j++) {
            if (j == remove_vertex) continue;

            int value;
            if (i == keep_vertex) {
                value = graph->matrix[keep_vertex][j] || graph->matrix[remove_vertex][j];
            }
            else if (j == keep_vertex) {
                value = graph->matrix[i][keep_vertex] || graph->matrix[i][remove_vertex];
            }
            else {
                value = graph->matrix[i][j];
            }

            // Ключевое отличие: если это была стягиваемая вершина, убираем петлю
            if (i == keep_vertex && j == keep_vertex) {
                // Оставляем только исходные петли, но не создаем новую из стягиваемого ребра
                value = graph->matrix[keep_vertex][keep_vertex] || graph->matrix[remove_vertex][remove_vertex];
            }

            new_graph->matrix[new_i][new_j] = value;
            new_j++;
        }
        new_i++;
    }

    return new_graph;
}

// Расщепление вершины (матричное)
Graph* split_vertex_matrix(Graph* graph, int v) {
    int new_vertices = graph->vertices + 1;
    Graph* new_graph = create_graph(new_vertices);
    int new_vertex = new_vertices - 1;

    printf("Расщепление вершины %d: создана вершина %d\n", v, new_vertex);

    // Копируем исходный граф
    for (int i = 0; i < graph->vertices; i++) {
        for (int j = 0; j < graph->vertices; j++) {
            new_graph->matrix[i][j] = graph->matrix[i][j];
        }
    }

    // Новая вершина соединяется со ВСЕМИ соседями исходной вершины
    for (int i = 0; i < graph->vertices; i++) {
        if (i != v && graph->matrix[v][i] == 1) {
            // Добавляем связь от новой вершины к соседу
            new_graph->matrix[new_vertex][i] = 1;
            new_graph->matrix[i][new_vertex] = 1;
        }
    }

    // Петля для новой вершины (если была у исходной)
    if (graph->matrix[v][v] == 1) {
        new_graph->matrix[new_vertex][new_vertex] = 1;
    }

    // Связь между расщепленными вершинами
    new_graph->matrix[v][new_vertex] = 1;
    new_graph->matrix[new_vertex][v] = 1;

    return new_graph;
}

// Функция для задания 2 - последовательные преобразования
void task2_sequential_operations() {
    printf("\n=== ЗАДАНИЕ 2 - ПОСЛЕДОВАТЕЛЬНЫЕ ПРЕОБРАЗОВАНИЯ ===\n\n");

    // Создаем начальный граф
    Graph* current_graph = create_graph(5);
    generate_random_graph(current_graph);

    printf("Начальный граф:\n");
    print_matrix(current_graph);

    int continue_operations = 1;
    while (continue_operations) {
        printf("Выберите операцию:\n");
        printf("1 - Отождествление вершин\n");
        printf("2 - Стягивание ребра\n");
        printf("3 - Расщепление вершины\n");
        printf("0 - Завершить задание 2\n");
        printf("Ваш выбор: ");

        int choice;
        scanf_s("%d", &choice);

        switch (choice) {
        case 0:
            continue_operations = 0;
            printf("Задание 2 завершено.\n");
            break;

        case 1: {
            int u, v;
            input_vertices_with_retry(current_graph, &u, &v, "отождествления");
            Graph* new_graph = identify_vertices_matrix(current_graph, u, v);
            if (new_graph != NULL) {
                free_graph(current_graph);
                current_graph = new_graph;
                printf("Результат отождествления:\n");
                print_matrix(current_graph);
            }
            break;
        }

        case 2: {
            int u, v;
            int valid_contraction = 0;
            while (!valid_contraction) {
                input_vertices_with_retry(current_graph, &u, &v, "стягивания ребра");
                Graph* new_graph = contract_edge_matrix(current_graph, u, v);
                if (new_graph != NULL) {
                    free_graph(current_graph);
                    current_graph = new_graph;
                    printf("Результат стягивания:\n");
                    print_matrix(current_graph);
                    valid_contraction = 1;
                }
                else {
                    printf("Попробуйте ввести другие вершины.\n\n");
                }
            }
            break;
        }

        case 3: {
            int w;
            input_vertex_with_retry(current_graph, &w, "расщепления вершины");
            Graph* new_graph = split_vertex_matrix(current_graph, w);
            if (new_graph != NULL) {
                free_graph(current_graph);
                current_graph = new_graph;
                printf("Результат расщепления вершины %d:\n", w);
                print_matrix(current_graph);
            }
            break;
        }

        default:
            printf("Неверный выбор. Попробуйте снова.\n\n");
            break;
        }
    }

    // Освобождаем память
    free_graph(current_graph);
}

// Объединение графов
Graph* union_graphs(Graph* g1, Graph* g2) {
    int min_vertices = (g1->vertices < g2->vertices) ? g1->vertices : g2->vertices;

    // Создаем копию большего графа
    Graph* result;
    if (g1->vertices >= g2->vertices) {
        result = create_graph(g1->vertices);
        for (int i = 0; i < g1->vertices; i++) {
            for (int j = 0; j < g1->vertices; j++) {
                result->matrix[i][j] = g1->matrix[i][j];
            }
        }
    }
    else {
        result = create_graph(g2->vertices);
        for (int i = 0; i < g2->vertices; i++) {
            for (int j = 0; j < g2->vertices; j++) {
                result->matrix[i][j] = g2->matrix[i][j];
            }
        }
    }

    // Объединяем с меньшим графом
    for (int i = 0; i < min_vertices; i++) {
        for (int j = 0; j < min_vertices; j++) {
            result->matrix[i][j] = result->matrix[i][j] || ((i < g1->vertices && j < g1->vertices) ? g1->matrix[i][j] : 0) ||
                ((i < g2->vertices && j < g2->vertices) ? g2->matrix[i][j] : 0);
            result->matrix[j][i] = result->matrix[i][j];
        }
    }

    return result;
}

// Пересечение графов 
Graph* intersection_graphs(Graph* g1, Graph* g2) {
    int min_vertices = (g1->vertices < g2->vertices) ? g1->vertices : g2->vertices;
    Graph* result = create_graph(min_vertices);

    for (int i = 0; i < min_vertices; i++) {
        for (int j = 0; j < min_vertices; j++) {
            result->matrix[i][j] = g1->matrix[i][j] && g2->matrix[i][j];
        }
    }

    return result;
}

// Кольцевая сумма графов
Graph* ring_sum_graphs(Graph* g1, Graph* g2) {
    int min_vertices = (g1->vertices < g2->vertices) ? g1->vertices : g2->vertices;
    Graph* result = create_graph(min_vertices);

    for (int i = 0; i < min_vertices; i++) {
        for (int j = 0; j < min_vertices; j++) {
            result->matrix[i][j] = (g1->matrix[i][j] && !g2->matrix[i][j]) || (!g1->matrix[i][j] && g2->matrix[i][j]);
        }
    }

    return result;
}

// Декартово произведение графов
Graph* cartesian_product(Graph* g1, Graph* g2) {
    int new_n = g1->vertices * g2->vertices;
    Graph* result = create_graph(new_n);

    for (int i1 = 0; i1 < g1->vertices; i1++) {
        for (int i2 = 0; i2 < g2->vertices; i2++) {
            for (int j1 = 0; j1 < g1->vertices; j1++) {
                for (int j2 = 0; j2 < g2->vertices; j2++) {
                    int idx1 = i1 * g2->vertices + i2;
                    int idx2 = j1 * g2->vertices + j2;

                    if ((i1 == j1 && g2->matrix[i2][j2] == 1) ||
                        (i2 == j2 && g1->matrix[i1][j1] == 1)) {
                        result->matrix[idx1][idx2] = 1;
                    }
                }
            }
        }
    }

    return result;
}

void task3_binary_operations_manual() {
    printf("\n=== ЗАДАНИЕ 3 - БИНАРНЫЕ ОПЕРАЦИИ ===\n\n");

    // Ввод размеров двух графов
    int vertices1, vertices2;

    printf("Введите количество вершин для первого графа G1: ");
    scanf_s("%d", &vertices1);
    printf("Введите количество вершин для второго графа G2: ");
    scanf_s("%d", &vertices2);

    // Создание и автоматическое заполнение графов
    Graph* g1 = create_graph(vertices1);
    Graph* g2 = create_graph(vertices2);

    generate_random_graph(g1);
    generate_random_graph(g2);

    printf("\n=== СГЕНЕРИРОВАННЫЕ ГРАФЫ ===\n");
    printf("Первый граф G1:\n");
    print_matrix(g1);
    printf("Второй граф G2:\n");
    print_matrix(g2);

    int continue_operations = 1;
    while (continue_operations) {
        printf("Выберите операцию для выполнения над графами:\n");
        printf("1 - Объединение G1 ∪ G2\n");
        printf("2 - Пересечение G1 ∩ G2\n");
        printf("3 - Кольцевая сумма G1 ⊕ G2\n");
        printf("4 - Декартово произведение G1 × G2\n");
        printf("0 - Завершить задание 4\n");
        printf("Ваш выбор: ");

        int choice;
        scanf_s("%d", &choice);

        switch (choice) {
        case 0:
            continue_operations = 0;
            printf("Задание 3 завершено.\n");
            break;

        case 1: {
            printf("\n=== ОБЪЕДИНЕНИЕ ГРАФОВ G1 ∪ G2 ===\n");
            Graph* result = union_graphs(g1, g2);
            printf("Результат объединения:\n");
            print_matrix(result);
            free_graph(result);
            break;
        }

        case 2: {
            printf("\n=== ПЕРЕСЕЧЕНИЕ ГРАФОВ G1 ∩ G2 ===\n");
            Graph* result = intersection_graphs(g1, g2);
            printf("Результат пересечения:\n");
            print_matrix(result);
            free_graph(result);
            break;
        }

        case 3: {
            printf("\n=== КОЛЬЦЕВАЯ СУММА ГРАФОВ G1 ⊕ G2 ===\n");
            Graph* result = ring_sum_graphs(g1, g2);
            printf("Результат кольцевой суммы:\n");
            print_matrix(result);
            free_graph(result);
            break;
        }

        case 4: {
            printf("\n=== ДЕКАРТОВО ПРОИЗВЕДЕНИЕ ГРАФОВ G1 × G2 ===\n");
            Graph* result = cartesian_product(g1, g2);
            printf("Результат декартова произведения:\n");
            printf("Результирующий граф имеет %d вершин\n", result->vertices);
            print_matrix(result);
            free_graph(result);
            break;
        }

        default:
            printf("Неверный выбор. Попробуйте снова.\n\n");
            break;
        }

        printf("\n");
    }

    // Освобождаем память
    free_graph(g1);
    free_graph(g2);
}

int main() {
    setlocale(LC_ALL, "Rus");
    srand(time(NULL));

    printf("=== ЗАДАНИЕ 1 ===\n\n");
    printf("1.1 Генерация матриц смежности:\n\n");

    int n1 = 4, n2 = 5;
    Graph* G1 = create_graph(n1);
    Graph* G2 = create_graph(n2);

    generate_random_graph(G1);
    generate_random_graph(G2);

    printf("Матрица M1 для графа G1:\n");
    print_matrix(G1);

    printf("Матрица M2 для графа G2:\n");
    print_matrix(G2);


    task2_sequential_operations();

    task3_binary_operations_manual();

    // Освобождаем память
    free_graph(G1);
    free_graph(G2);

    getchar();
    getchar();
    return 0;
}
