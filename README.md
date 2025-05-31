# 📦 Projeto Estoque com Árvore B

Projeto acadêmico desenvolvido em linguagem C para gerenciar um sistema de estoque, utilizando uma **Árvore B** como índice primário para tornar as buscas mais eficientes. O sistema realiza persistência de dados em arquivos texto e binário.

---

## 🧠 Objetivo

Desenvolver um sistema de controle de estoque com suporte a operações de inserção, remoção, atualização e busca, aplicando conceitos de estrutura de dados (Árvore B) e manipulação de arquivos. Este projeto tem fins acadêmicos e visa aprofundar os conhecimentos em programação e organização de dados.

---

## ✅ Funcionalidades

- Inserção de novo produto
- Remoção de produto pelo código
- Alteração de dados de um produto
- Busca eficiente com uso de Árvore B
- Persistência dos dados e do índice em arquivos

---

## 📌 Formato dos Dados

Os dados são armazenados em dois arquivos:

- `dados.txt` — contém os registros dos produtos, como:
  ```
  CÓDIGO|NOME|QUANTIDADE|PREÇO
  ```
- `indice.idx` — arquivo binário com a árvore B, armazenando:
  - Chave primária (código do produto)
  - Posição do registro no arquivo de dados

---

## 📚 Tecnologias Utilizadas

- Linguagem C
- Estrutura de dados: Árvore B
- Manipulação de arquivos texto e binário
