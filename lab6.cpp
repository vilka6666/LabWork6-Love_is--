#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <locale.h>
#include <algorithm>

using namespace std;

vector < vector < int>> mergeVertices(const vector < vector < int>>& G, int vertex1, int vertex2) {
	vector < vector < int>> result = G;

	// Добавление соседей vertex2 к списку смежности vertex1, если они уникальны
	for (int neighbor : G[vertex2]) {
		if (neighbor != vertex1) {
			// Добавляем только уникальных соседей
			if (find(result[vertex1].begin(), result[vertex1].end(), neighbor) == result[vertex1].end()) {
				result[vertex1].push_back(neighbor);
			}
		}
	}

	// Перенаправление ребер, идущих к vertex2, на vertex1
	for (int i = 0; i < result.size(); ++i) {
		for (int j = 0; j < result[i].size(); ++j) {
			if (result[i][j] == vertex2) {
				result[i][j] = vertex1;
				// Удаление дубликатов, если они появились после перенаправления
				if (i == vertex1) {
					result[i].erase(unique(result[i].begin(), result[i].end()), result[i].end());
				}
			}
		}
	}

	// Удаление петель у vertex1, если они есть
	result[vertex1].erase(remove(result[vertex1].begin(), result[vertex1].end(), vertex1), result[vertex1].end());

	// Удаление vertex2 из списка смежности всех вершин
	for (auto& neighbors : result) {
		neighbors.erase(remove(neighbors.begin(), neighbors.end(), vertex2), neighbors.end());
	}

	// Удаление списка смежности для vertex2
	result.erase(result.begin() + vertex2);

	return result;
}

vector < vector < int>> generateAndPrintMatrix(int num_vertices, double edge_probability) {
	vector < vector < int>> adjacency_matrix(num_vertices + 1, vector<int>(num_vertices + 1, 0));
	int num_edges = 0;

	for (int i = 1; i <= num_vertices; ++i) {
		for (int j = i + 1; j <= num_vertices; ++j) {
			if (static_cast<double>(rand()) / RAND_MAX < edge_probability) {
				adjacency_matrix[i][j] = 1;
				adjacency_matrix[j][i] = 1;
				num_edges++;
			}
		}
	}

	printf("Матрица смежности:\n");
	for (int i = 1; i <= num_vertices; ++i) {
		for (int j = 1; j <= num_vertices; ++j) {
			printf("%d ", adjacency_matrix[i][j]);
		}
		printf("\n");
	}
	return adjacency_matrix;
}

vector < vector < int>> matrixToList(const vector < vector < int>>& matrix) {
	int num_vertices = matrix.size() - 1;
	vector < vector < int>> adjacency_list(num_vertices + 1);

	for (int i = 1; i <= num_vertices; ++i) {
		for (int j = 1; j <= num_vertices; ++j) {
			if (matrix[i][j] == 1) {
				adjacency_list[i].push_back(j);
			}
		}
	}

	return adjacency_list;
}

void printList(const vector < vector < int>>& list) {
	printf("Список смежности:\n");
	for (int i = 1; i < list.size(); ++i) {
		printf("%d: ", i);
		for (const int& j : list[i]) {
			printf("%d ", j);
		}
		printf("\n");
	}
}

void printMatrix(const vector < vector < int>>& matrix) {
	printf("Матрица смежности:\n");
	for (int i = 1; i < matrix.size(); i++) {
		for (int j = 1; j < matrix[i].size(); j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
}

//отождествления вершин
void identifyVertices(const vector < vector < int>>& originalMatrix, const vector < vector < int>>& originalList, int vertex1, int vertex2, vector < vector < int>>& resultMatrix, vector < vector < int>>& resultList) {
	// Создание копий исходной матрицы и списка смежности
	resultMatrix = originalMatrix;
	resultList = originalList;

	int n = resultMatrix.size();

	// Обновление копии матрицы смежности
	for (int i = 0; i < n; ++i) {
		if (resultMatrix[vertex2][i]) {
			resultMatrix[vertex1][i] = 1;
			resultMatrix[i][vertex1] = 1;
		}
		resultMatrix[vertex2][i] = 0;
		resultMatrix[i][vertex2] = 0;
	}


	// Обновление копии списка смежности
	for (int neighbor : resultList[vertex2]) {
		if (find(resultList[vertex1].begin(), resultList[vertex1].end(), neighbor) == resultList[vertex1].end() && neighbor != vertex1) {
			resultList[vertex1].push_back(neighbor);
		}
	}

	// Удаление vertex2 из всех списков смежности в копии
	for (auto& neighbors : resultList) {
		auto it = remove(neighbors.begin(), neighbors.end(), vertex2);
		neighbors.erase(it, neighbors.end());
		for (auto& neighbor : neighbors) {
			if (neighbor > vertex2) {
				--neighbor;
			}
		}
	}

	// Удаление vertex2 из копии матрицы
	resultMatrix.erase(resultMatrix.begin() + vertex2);
	for (auto& row : resultMatrix) {
		row.erase(row.begin() + vertex2);
	}

	// Удаление списка смежности для vertex2 и обновление последующих индексов в копии
	resultList.erase(resultList.begin() + vertex2);
}


//стягивания ребра
void contractEdge(const vector<vector<int>>& originalMatrix, const vector<vector<int>>& originalList, int vertex1, int vertex2, vector<vector<int>>&
	resultMatrix, vector<vector<int>>& resultList) {
	resultMatrix = originalMatrix;
	resultList = originalList;

	int n = resultMatrix.size();

	// Проверка на наличие ребра между vertex1 и vertex2
	if (!resultMatrix[vertex1][vertex2] && !resultMatrix[vertex2][vertex1]) {
		printf("Нет ребра между %d и %d для стягивания.\n", vertex1, vertex2);
		return;
	}

	// Объединение вершин в копии матрицы смежности
	for (int i = 0; i < n; ++i) {
		if (resultMatrix[vertex2][i]) {
			resultMatrix[vertex1][i] = 1;
			resultMatrix[i][vertex1] = 1;
		}
		resultMatrix[vertex2][i] = 0;
		resultMatrix[i][vertex2] = 0;
	}
	resultMatrix[vertex1][vertex1] = 0;

	// Объединение списков смежности
	for (int neighbor : resultList[vertex2]) {
		if (find(resultList[vertex1].begin(), resultList[vertex1].end(), neighbor) == resultList[vertex1].end() && neighbor != vertex1) {
			resultList[vertex1].push_back(neighbor);
		}
	}

	// Удаление vertex2 из всех списков смежности в копии
	for (auto& neighbors : resultList) {
		auto it = remove(neighbors.begin(), neighbors.end(), vertex2);
		neighbors.erase(it, neighbors.end());
	}

	// Удаление vertex2 из копии матрицы и списка смежности
	resultMatrix.erase(resultMatrix.begin() + vertex2);
	for (auto& row : resultMatrix) {
		row.erase(row.begin() + vertex2);
	}
	resultList.erase(resultList.begin() + vertex2);
}

//расщепление вершины
void splitVertex(vector < vector < int>>& originalMatrix, vector < vector < int>>& originalList, int vertex, vector < vector < int>>& resultMatrix, vector < vector < int>>& resultList) {
	int n = originalMatrix.size();

	// Создаем новую матрицу смежности и новый список смежности
	resultMatrix.resize(n + 1, vector<int>(n + 1, 0));
	resultList.resize(n + 1);

	// Копируем исходную матрицу смежности и список смежности в новые структуры
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			resultMatrix[i][j] = originalMatrix[i][j];
		}
		resultList[i] = originalList[i];
	}

	// Копируем вершину, которую отождествляем
	for (int i = 0; i < n; ++i) {
		resultMatrix[i][n] = originalMatrix[i][vertex];
		resultMatrix[n][i] = originalMatrix[vertex][i];
	}

	// Добавляем новую вершину в список смежности
	for (int i = 0; i < n; ++i) {
		if (i != vertex) {
			resultList[i].push_back(n);
		}
	}

	// Добавляем новую вершину в конец списка смежности
	for (int i = 0; i < originalList[vertex].size(); ++i) {
		resultList[n].push_back(originalList[vertex][i]);
	}
}

//объединения
vector < vector < int>> mergeGraphs(const vector < vector < int>>& graph1, const vector < vector < int>>& graph2) {
	int maxSize = max(graph1.size(), graph2.size());

	// Создание новой матрицы смежности для объединенного графа
	vector < vector < int>> mergedGraph(maxSize, vector<int>(maxSize, 0));

	// Копирование связей из первого графа
	for (int i = 0; i < graph1.size(); ++i) {
		for (int j = 0; j < graph1[i].size(); ++j) {
			mergedGraph[i][j] = graph1[i][j];
		}
	}

	// Добавление или перезапись связей из второго графа
	for (int i = 0; i < graph2.size(); ++i) {
		for (int j = 0; j < graph2[i].size(); ++j) {
			if (graph2[i][j] > 0) {
				mergedGraph[i][j] = graph2[i][j];
			}
		}
	}

	return mergedGraph;
}

//пересечения
vector < vector < int>> intersectGraphs(const vector < vector < int>>& graph1, const vector < vector < int>>& graph2) {
	int size = min(graph1.size(), graph2.size());
	vector < vector < int>> intersectionGraph(size, vector<int>(size, 0));

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			// Элемент в новой матрице будет 1, если соответствующие элементы обеих матриц равны 1
			intersectionGraph[i][j] = (graph1[i][j] && graph2[i][j]);
		}
	}

	return intersectionGraph;
}

//кольцевой суммы
vector < vector < int>> adaptiveRingSumGraphs(const vector < vector < int>>& graph1, const vector < vector < int>>& graph2) {
	int maxSize = max(graph1.size(), graph2.size());
	vector < vector < int>> ringSumGraph(maxSize, vector<int>(maxSize, 0));

	for (int i = 0; i < maxSize; ++i) {
		for (int j = 0; j < maxSize; ++j) {
			int val1 = (i < graph1.size() && j < graph1.size()) ? graph1[i][j] : 0;
			int val2 = (i < graph2.size() && j < graph2.size()) ? graph2[i][j] : 0;
			ringSumGraph[i][j] = val1 != val2;
		}
	}

	return ringSumGraph;
}


//декартова произведения
vector < vector < int>> cartesianProductGraphs(const vector < vector < int>>& graph1, const vector < vector < int>>& graph2) {
	int n1 = graph1.size() - 1; // Учитываем, что размеры матриц начинаются с 1
	int n2 = graph2.size() - 1;
	int productSize = n1 * n2;
	vector < vector < int>> productGraph(productSize + 1, vector<int>(productSize + 1, 0)); // +1 для корректного размера

	for (int i1 = 1; i1 <= n1; ++i1) {
		for (int i2 = 1; i2 <= n2; ++i2) {
			for (int j1 = 1; j1 <= n1; ++j1) {
				for (int j2 = 1; j2 <= n2; ++j2) {
					int v1 = (i1 - 1) * n2 + i2; // Пересчитываем индексы
					int v2 = (j1 - 1) * n2 + j2;
					if (i1 == j1 && graph2[i2][j2]) {
						productGraph[v1][v2] = 1;
					}
					if (i2 == j2 && graph1[i1][j1]) {
						productGraph[v1][v2] = 1;
					}
				}
			}
		}
	}

	return productGraph;
}


int main() {
	setlocale(LC_ALL, "RUS");
	int num_vertices, operation;
	int graph_choice, vertex1, vertex2;
	double edge_probability;

	vector < vector < int>> matrix1, matrix2, matrixCopy;
	vector < vector < int>> list1, list2, listCopy;

	srand(static_cast<unsigned int>(time(nullptr)));

	while (true) {
		printf("Выберите операцию:\n 1 - Сгенерировать графы\n 2 - Отождествление\n 3 - Стягивание ребра\n 4 - Расщепление вершины\n 5 - Объединение матриц 1 и 2\n 6 - Пересечение графов 1 и 2\n 7 - Кольцевая сумма графов 1 и 2\n 8 - Декартово произведение графов 1 и 2\n 9 - Выход\n");
		//scanf("%d", &operation);
		if (scanf("%d", &operation) != 1) {
			printf("Ошибка: ожидался ввод числа.\n");
			while (getchar() != '\n'); // Очистка ввода
			continue;
		}

		if (operation == 9) {
			break;
		}

		switch (operation) {
		case 1: {

			printf("Введите количество вершин в первом графе: ");
			scanf("%d", &num_vertices);
			printf("Введите вероятность наличия ребра в первом графе (0 - 1): ");
			scanf("%lf", &edge_probability);

			printf("Первый граф:\n");
			matrix1 = generateAndPrintMatrix(num_vertices, edge_probability);
			list1 = matrixToList(matrix1);
			printList(list1);

			printf("Введите количество вершин во втором графе: ");
			scanf("%d", &num_vertices);
			printf("Введите вероятность наличия ребра во втором графе (0 - 1): ");
			scanf("%lf", &edge_probability);

			printf("Второй граф:\n");
			matrix2 = generateAndPrintMatrix(num_vertices, edge_probability);
			list2 = matrixToList(matrix2);
			printList(list2);
			break;
		}
		case 2:
		case 3: {
			printf("Введите номер графа для выполнения операции (1 или 2): ");
			scanf("%d", &graph_choice);
			printf("Введите номера двух вершин (например, 1 3): ");
			scanf("%d %d", &vertex1, &vertex2);
			if (graph_choice == 1) {
				if (operation == 2) {
					identifyVertices(matrix1, list1, vertex1, vertex2, matrixCopy, listCopy);
				}
				else {
					contractEdge(matrix1, list1, vertex1, vertex2, matrixCopy, listCopy);
				}
				printMatrix(matrixCopy);
				printList(matrixToList(matrixCopy));
			}
			else if (graph_choice == 2) {
				if (operation == 2) {
					identifyVertices(matrix2, list2, vertex1, vertex2, matrixCopy, listCopy);
				}
				else {
					contractEdge(matrix2, list2, vertex1, vertex2, matrixCopy, listCopy);
				}
				printMatrix(matrixCopy);
				printList(matrixToList(matrixCopy));
			}
			break;
		}
		case 4: {
			printf("Введите номер графа для выполнения операции (1 или 2): ");
			scanf("%d", &graph_choice);
			printf("Введите номер вершины для расщепления: ");
			scanf("%d", &vertex1);
			if (graph_choice == 1) {
				splitVertex(matrix1, list1, vertex1, matrixCopy, listCopy);
				printMatrix(matrixCopy);
				printList(matrixToList(matrixCopy));
			}
			else if (graph_choice == 2) {
				splitVertex(matrix2, list2, vertex1, matrixCopy, listCopy);
				printMatrix(matrixCopy);
				printList(matrixToList(matrixCopy));
			}
			break;
		case 5: {
			vector < vector < int>> mergedGraph = mergeGraphs(matrix1, matrix2);
			printMatrix(mergedGraph);
			break;
		}
		case 6: {
			vector < vector < int>> intersectionGraph = intersectGraphs(matrix1, matrix2);
			printMatrix(intersectionGraph);
			break;
		}
		case 7: {
			vector < vector < int>> ringSumGraph = adaptiveRingSumGraphs(matrix1, matrix2);
			printMatrix(ringSumGraph);
			break;
		}
		case 8: {
			vector < vector < int>> cartesianProductGraph = cartesianProductGraphs(matrix1, matrix2);
			printMatrix(cartesianProductGraph);
			break;
		}
		}
		default:
			printf("Неверный выбор операции.\n");
			break;
		}
	}
	return 0;
}