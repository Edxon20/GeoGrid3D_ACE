/**
 * GeoGrid3D - Analizador Lexico Standalone (CLI)
 * Materia: Compiladores - Escuela de Ingenieria de Sistemas (ULA)
 * ACE: Edixon Toro (C.I. 27364279) y Mikael Pizzi (C.I. 27777243)
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <geogrid_lexer.hpp>

using namespace std;
using namespace GeoGrid;

// Lee el contenido completo de un archivo a un std::string
string read_file_content(const string& filepath)
{
    ifstream file(filepath);
    if (!file.is_open())
    {
        throw runtime_error("No se pudo abrir el archivo: " + filepath);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Imprime los tokens reconocidos en formato tabular
void print_token_table(const DynArray<GeoGridToken>& tokens)
{
    cout << left 
         << setw(8)  << "LINEA" 
         << setw(8)  << "COL" 
         << setw(10) << "OFFSET"
         << setw(22) << "CATEGORIA" 
         << "LEXEMA" << "\n";
    cout << string(65, '-') << "\n";

    for (nat_t i = 0; i < tokens.size(); ++i)
    {
        const auto& t = tokens[i];
        cout << left 
             << setw(8)  << t.line
             << setw(8)  << t.column
             << setw(10) << t.position
             << setw(22) << t.name 
             << "\"" << t.lexeme << "\"\n";
    }
    cout << string(65, '-') << "\n";
    cout << "Total de tokens significativos: " << tokens.size() << "\n\n";
}

// Procesa una cadena de codigo fuente e imprime resultados o errores
bool process_source(const Lexer& lexer, const string& source, const string& origin_name)
{
    cout << ____________________________________________________________________n";
    cout << "Analizando: " << origin_name << "\n";
    cout << ____________________________________________________________________n";

    try
    {
        DynArray<GeoGridToken> tokens = tokenize_geogrid(lexer, source, true);
        cout << "[EXITO] Analisis lexico completado sin errores.\n\n";
        print_token_table(tokens);
        return true;
    }
    catch (const exception& ex)
    {
        cout << "[ERROR LEXICO DETECTADO]\n";
        cerr << "Detalle: " << ex.what() << "\n\n";
        return false;
    }
}

int main(int argc, char* argv[])
{
    cout << ____________________________________________________________________n";
    cout << " GeoGrid3D - Analizador Lexico (DeSiGNAR C++17)\n";
    cout << " Compiladores - ULA | Equipo ACE: Edixon Toro y Mikael Pizzi\n";
    cout << ____________________________________________________________________n\n";

    // Inicializar el lexer con las reglas de GeoGrid3D
    cout << "Inicializando motor de automatas finitos y tablas de transicion...\n";
    Lexer lexer = build_geogrid_lexer();
    cout << "Motor lexico configurado correctamente.\n\n";

    if (argc > 1)
    {
        string arg = argv[1];

        // Modo suite de pruebas automatizada
        if (arg == "--test" || arg == "--all")
        {
            const string test_names[] = {
                "nivel_terror.gg",
                "mazmorra_nivel.gg",
                "error_lexico.gg"
            };

            for (const auto& fname : test_names)
            {
                // Intentar ruta directa o relativa al repositorio
                string candidate1 = "examples/" + fname;
                string candidate2 = "MAIN/GeoGrid3D/GeoGrid3D_ACE/examples/" + fname;
                string chosen = candidate1;

                ifstream check(candidate1);
                if (!check.is_open())
                {
                    chosen = candidate2;
                }

                try
                {
                    string content = read_file_content(chosen);
                    process_source(lexer, content, chosen);
                }
                catch (const exception& e)
                {
                    cerr << "Error cargando prueba (" << fname << "): " << e.what() << "\n\n";
                }
            }
            return 0;
        }

        // Modo archivo especifico
        try
        {
            string content = read_file_content(arg);
            bool ok = process_source(lexer, content, arg);
            return ok ? 0 : 1;
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << "\n";
            return 1;
        }
    }

    // Modo interactivo / por defecto
    cout << "Uso: ./geogrid_lexer <archivo.gg>\n";
    cout << "     ./geogrid_lexer --test\n\n";
    cout << "Ejecutando demostracion con codigo por defecto:\n";

    const string demo_source = 
        "// Prueba por defecto\n"
        "grid {\n"
        "    size: (20, 20);\n"
        "    cell_size: 2.0;\n"
        "}\n"
        "terrain {\n"
        "    default: \"pasto_oscuro\";\n"
        "    paint (10, 10) = \"#8B0000\";\n"
        "}\n"
        "entities {\n"
        "    limit \"estatua\" <= 4;\n"
        "    spawn \"boss\" at center;\n"
        "}\n";

    process_source(lexer, demo_source, "Buffer en memoria (Demo)");
    return 0;
}
