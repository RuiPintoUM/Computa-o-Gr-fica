Criação da struct 

struct Patch {
    std::vector<int> indices;
    std::vector<std::vector<double>> controlPoints;
};

para armazenar os indices e controlPoints para depois construir os patches 

1 faze dar o parser do ficheiro