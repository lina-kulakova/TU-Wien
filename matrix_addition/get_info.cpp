
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>

#define BUF_LEN 1024
#define SIZE 26

using std::string;
using std::cout;
using std::endl;


void write_data_to_array (const string &filename, int array_loop[SIZE], int array_templ[SIZE], int i)
{
  FILE *fp;
  char buf[BUF_LEN];
  if (!(fp = fopen (filename.c_str (), "r")))
    {
      cout << "Cannot open file " << filename << endl;
      return;
    }
  int count = 0;
  while (fgets (buf, BUF_LEN, fp) && count < 2)
    {
      char *p;
      if (strstr (buf, "add_matrices_loop") && (p = strchr (buf, '%')) && (strstr (buf, "[.]")))  
        {
          p++;
          while (*p == ' ') p++;
          array_loop[i] = atoi (p);
          count++;
        }

      if (strstr (buf, "add_matrices_template") && (p = strchr (buf, '%')) && strstr (buf, "[.]"))
        {
          p++;
          while (*p == ' ') p++;
          array_templ[i] = atoi (p);
          count++;
        }
    }
}


int main ()
{
  char buf[BUF_LEN];
  FILE *instr, *cycles;
  int instr_array_loop[SIZE];
  int instr_array_templ[SIZE];
  int cycles_array_loop[SIZE];
  int cycles_array_templ[SIZE];

  for (int i = 0; i < SIZE; ++i)
    {
      string instr_file ("./instructions/out_" + std::to_string (i));
      write_data_to_array (instr_file, instr_array_loop, instr_array_templ, i);

      string cycles_file ("./cycles/out_" + std::to_string (i));
      write_data_to_array (cycles_file, cycles_array_loop, cycles_array_templ, i);
    }

  cout << "loop instructions:" << endl;
  for (int i = 0; i < SIZE; ++i)
    cout << instr_array_loop[i] << " ";
  cout << endl << endl;

  cout << "loop cycles:" << endl;
  for (int i = 0;  i < SIZE; ++i)
    cout << cycles_array_loop[i] << " ";
  cout << endl << endl;

  cout << "loop IPC:" << endl;
  for (int i = 0;  i < SIZE; ++i)
    printf ("%.2lf ", (double) instr_array_loop[i] / cycles_array_loop[i]);
  cout << endl << endl;

  cout << "template instructions:" << endl;
  for (int i = 0; i < SIZE; ++i)
    cout << instr_array_templ[i] << " ";
  cout << endl << endl;

  cout << "template cycles:" << endl;
  for (int i = 0;  i < SIZE; ++i)
    cout << cycles_array_templ[i] << " ";
  cout << endl << endl;

  cout << "template IPC:" << endl;
  for (int i = 0;  i < SIZE; ++i)
    printf ("%.2lf ", (double) instr_array_templ[i] / cycles_array_templ[i]);
  cout << endl << endl;
}

