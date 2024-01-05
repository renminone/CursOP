#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <locale.h>
#define FIRST_LINE_SIZE 30
#define SEPORATOR ","
#define INFINITY 100000000

typedef struct
{
  char* from;
  time_t time_depart;
  char* to;
  time_t time_arrive;
  int cost;
} Row;

typedef struct
{
  char* name;
  int val;
  int isUsed;
} City;

int max_line = 190;
int database_size;
Row* Database;

int nodes_size = 1;
City* Nodes = NULL;

void addCity(char* name)
{
  if (!Nodes)
  {
     Nodes = calloc(nodes_size, sizeof(City));
     Nodes[nodes_size - 1].name = name;
     return;
  }
  
  for (int i = 0; i < nodes_size; i++)
  {
     if (!strcmp(name, Nodes[i].name))
     return;
  }

  Nodes = (City*)realloc(Nodes, ++nodes_size * sizeof(City));
  Nodes[nodes_size - 1].name = name;
}

int loadDatabase(FILE* fin)
{
  if (!fin) return 1;
  if (Database) free(Database);

  char* line = (char*)calloc(FIRST_LINE_SIZE, sizeof(char));
  char* val = fgets(line, FIRST_LINE_SIZE - 1, fin);
  int _size = strtol(val, &val, 10);
  max_line = strtol(val, &val, 10);
  database_size = _size;
  Database = (Row*)calloc(database_size, sizeof(Row));
  free(line);
  line = (char*)calloc(max_line, sizeof(char));
  char* ptr;

  for (int i = 0; i < database_size; i++)
  {
     fgets(line, max_line - 1, fin);
     val = line;
     ptr = val;

     ptr = strstr(val, SEPORATOR);
     Database[i].from = calloc(ptr - val + 1, sizeof(char));
     memcpy(Database[i].from, val, ptr - val);
     val = ++ptr;
     ptr = strstr(val, SEPORATOR);
     time_t time_to_depart = strtol(val, NULL, 10);
     Database[i].time_depart = time_to_depart;
     val = ++ptr;
     ptr = strstr(val, SEPORATOR);
     Database[i].to = calloc(ptr - val + 1, sizeof(char));
     memcpy(Database[i].to, val, ptr - val);
     val = ++ptr;
     ptr = strstr(val, SEPORATOR);
     time_t time_to_arrive = strtol(val, NULL, 10);
     Database[i].time_arrive = time_to_arrive;
     val = ++ptr;
     ptr = strstr(val, SEPORATOR);
     Database[i].cost = strtol(val, NULL, 10);
 
     addCity(Database[i].from);
     addCity(Database[i].to);
  }
  free(line);
  return 0;
}

int saveDatabase(FILE* fout)
{
  if (!fout) return 1;
  fprintf(fout, "%d %d\n", database_size, max_line);
  for (int i = 0; i < database_size; i++)
      fprintf(fout, "%s,%ld,%s,%ld,%d\n", Database[i].from,
  Database[i].time_depart, Database[i].to, Database[i].time_arrive,
  Database[i].cost);
  fclose(fout);
  return 0;
}

int createRow(char* from, time_t time_depart, char* to, time_t time_arrive, int cost)
{
  if (!database_size)
    Database = (Row*)calloc(++database_size, sizeof(Row));
  else
    Database = (Row*)realloc(Database, ++database_size * sizeof(Row));
  if (!Database) return 1;

  Database[database_size - 1].from = from;
  Database[database_size - 1].time_depart = time_depart;
  Database[database_size - 1].to = to;
  Database[database_size - 1].time_arrive = time_arrive;
  Database[database_size - 1].cost = cost;
  
  addCity(from);
  addCity(to);
  return 0;
}

int updateRow(int index, char* from, time_t time_depart, char* to, time_t time_arrive, int cost)
{
  if (index >= database_size)
    return 1;
  Database[index].from = from;
  Database[index].time_depart = time_depart;
  Database[index].to = to;
  Database[index].time_arrive = time_arrive;
  Database[index].cost = cost;
  
  addCity(from);
  addCity(to);
  return 0;
}

int deleteRow(int index)
{
  if (index >= database_size)
    return 1;
  //Let the memory leek
  memcpy(Database + index, Database + index + 1, database_size - index - 1);
  Database = (Row*)realloc(Database, --database_size * sizeof(Row));
  return 0;
}

int findRow(char* from, time_t time_depart, char* to, time_t time_arrive, int cost)
{
  for (int i = 0; i < database_size; i++)
  {
      if (!strcmp(Database[i].from, from) && Database[i].time_depart ==
      time_depart && !strcmp(Database[i].from, from) && Database[i].time_arrive ==
      time_arrive && Database[i].cost == cost)
      return i;
  }
  return -1;
}

int findNode(char* name)
{
  for (int i = 0; i < nodes_size; i++)
      if (!strcmp(name, Nodes[i].name))
          return i;
  return -1;
}

City* findMin()
{
  int min = INFINITY;
  int min_pos = 0;
  for (int i = 0; i < nodes_size; i++)
  {
     if ((!Nodes[i].isUsed) && (Nodes[i].val < min))
     {
         min = Nodes[i].val;
         min_pos = i;
     }
  }
  if (min == INFINITY) return NULL;
      return Nodes + min_pos;
}

int cheap(Row _row)
{
    return _row.cost;
}

int fast(Row _row)
{
    return _row.time_arrive - _row.time_depart;
}

int findMinRoute(char* _from, char* _to, int (*whightdun)(Row _row))
{
    int* P = calloc(nodes_size, sizeof(int));
    for (int i = 0; i < nodes_size; i++)
    {
        Nodes[i].isUsed = 0;
        if (!strcmp(_from, Nodes[i].name))
            Nodes[i].val = 0;
        else
            Nodes[i].val = INFINITY;
    }
    City* w = findMin();
    while (w != NULL)
    {
        for (int i = 0; i < database_size; i++)
        {
            if (!strcmp(Database[i].from, w->name))
            {
                int c = findNode(Database[i].to);
                if (Nodes[c].val > whightdun(Database[i]) + w->val)
                {
                    Nodes[c].val = whightdun(Database[i]) + w->val;
                    P[c] = w - Nodes;
                }
            }
        }
        w->isUsed = 1;
        w = findMin();
     }
  int from = findNode(_from);
  int origin = findNode(_to);
  int to = origin;
  int ret = Nodes[to].val;
  printf("%s \n", Nodes[to].name);
  while (to != from)
  {
      if (P[to] == to && to != origin)
      {
          printf("NO ROUTE TO.\n");
          return -1;
      }
      printf("%s <- ", Nodes[to].name);
      to = P[to];
  }
  printf("%s \n", _from);
  return ret;
}

//Вывод записей
void prin_db()
{
    printf(" | |%-3s| %-25s | %-25s | %-25s | %-25s | %-25s | | \n", "ID", "Пункт отправления", "Время отправления", "Пункт прибытия", "Время прибытия", "Стоимость");
    for (int i = 0; i < database_size; i++)
        printf(" | |%-3d| %-25s | %-25ld | %-25s | %-25ld | %-25d | | \n", i, Database[i].from, Database[i].time_depart, Database[i].to,
    Database[i].time_arrive, Database[i].cost);
    return;
}

#ifndef MAIN
#define MAIN
int main()
{
    setlocale(LC_ALL, "Rus");
    char command[100];
    while (1)
    {
        printf("\n> ");
        scanf("%s", command);
        if (!strcmp("list", command))
        {
            prin_db();
            printf("\n");
            continue;
        }
        if (!strcmp("save", command))
        {
            char filename[100];
            scanf("%s", filename);
            printf("%s", saveDatabase(fopen(filename, "a")) ? "\nERROR." : "\nOK.");
            continue;
        }
        if (!strcmp("load", command))
        {
            char filename[100];
            scanf("%s", filename);
            printf("%s", loadDatabase(fopen(filename, "r")) ? "\nERROR." : "\nOK.");
            continue;
        }
        if (!strcmp("create", command))
        {
            char* from = (char*)calloc(121, sizeof(char));
            time_t depart;
            char* to = (char*)calloc(121, sizeof(char));
            time_t arrive;
            int cost;
            scanf("%s %ld %s %ld %d", from, &depart, to, &arrive, &cost);
            printf("%s", createRow(from, depart, to, arrive, cost) ? "\nERROR." : "\nOK.");
            continue;
        }
        if (!strcmp("remove", command))
        {
            int index;
            scanf("%d", &index);
            printf("%s", deleteRow(index) ? "\nERROR." : "\nOK.");
            continue;
        }
        if (!strcmp("modify", command))
        {
            int index;
            char* from = (char*)calloc(121, sizeof(char));
            time_t depart;
            char* to = (char*)calloc(121, sizeof(char));
            time_t arrive;
            int cost;
            scanf("%d %s %ld %s %ld %d", &index, from, &depart, to, &arrive, &cost);
            printf("%s", updateRow(index, from, depart, to, arrive, cost) ? "\nERROR." : "\nOK.");
            continue;
        }
        if (!strcmp("find", command))
        {
            char method[100];
            char from[100];
            char to[100];
            scanf("%s %s %s", method, from, to);
            if (!strcmp("cheap", method))
            {
                int ret = findMinRoute(from, to, (*cheap));
                if (~ret)
                    printf("Стоимость маршрута: %d$\nOK.", ret);
                else
                    printf("\nERROR.");
                continue;
            }
            if (!strcmp("fast", method))
            {
                int ret = findMinRoute(from, to, (*fast));
                if (~ret)
                    printf("Время маршрута: %d у.е.в\nOK.", ret);
                else
                    printf("\nERROR.");
                continue;
            }
            printf("Неверный параметр. Используйте cheap или fast.\nERROR.");
            continue;
   }
   if (!strcmp("help", command))
   {
       printf("help - помощь.\n");
       printf("list - вывод всех записей.\n");
       printf("save - сохранение базы данных (возможно ошибка!).\n");
       printf("create - создание записи в базе данных.\n");
       printf("modify - редактирование записи.\n");
       printf("remove - удаление записи из базы данных\n");
       printf("find - поиск определённых записей (cheap - самый дешёвый, fast - самый быстрый)\n");
       printf("quit - выход из программы.\n");
       continue;
    }
    if (!strcmp("quit", command)) break;
    printf("Команда не распознана, вызовите помощь через help. \nERROR.");
  }
}
