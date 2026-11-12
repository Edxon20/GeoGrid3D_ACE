/**
 * GeoGrid3D - Analizador Lexico (Lexer)
 * Compiladores - Escuela de Ingenieria de Sistemas (ULA)
 * ACE: Edixon Toro (V027364279) y Mikael Pizzi (V027777243)
 * DeSiGNAR (C++17)
 */
#include <lexer.hpp>
#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iomanip>


namespace GeoGrid
{
    using namespace Designar;
    
    struct GeoGridToken
    {
        std::string name;       // Categoria lexica 
        std::string lexeme;     // Texto exacto consumido del archivo fuente
        nat_t position;         // Offset absoluto en bytes
        nat_t line;             // Numero de linea comienza en 1
        nat_t column;           // Numero de columna comienza en 1
    };

    /**
     * Construye el objeto Lexer de DeSiGNAR con todas las reglas lexicas del lenguaje GeoGrid3D.
     * add_token() define la prioridad
     * en caso de empates en la regla de Coincidencia Mas Larga -Maximal Munch-
     */


    inline Lexer build_geogrid_lexer()
    {
        Lexer lexer;

        // ________________________________________________________________________
        // 1. Conjuntos Auxiliares (DeSiGNAR no soporta clases [a-z] ni [0-9])
        // ________________________________________________________________________
        const std::string LOWER =
            "a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z";
        const std::string UPPER =
            "A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z";
        const std::string DIGIT =
            "0|1|2|3|4|5|6|7|8|9";
        
        const std::string LETTER =
            "(" + LOWER + "|" + UPPER + "|_)";

        const std::string ALPHANUM =
            "(" + LOWER + "|" + UPPER + "|_|" + DIGIT + ")";

        // ________________________________________________________________________
        // 2. palabras reservadas 
        // ________________________________________________________________________

        // -  Bloques Estructurales -
        lexer.add_token("KEYWORD_GRID", "grid");
        lexer.add_token("KEYWORD_TERRAIN", "terrain");
        lexer.add_token("KEYWORD_ENTITIES", "entities");

        // - Atributos de configuracion -
        lexer.add_token("KEYWORD_SIZE", "size");
        lexer.add_token("KEYWORD_CELL_SIZE", "cell_size");
        lexer.add_token("KEYWORD_DEFAULT", "default");

        // - Terreno y Pintura -
        lexer.add_token("KEYWORD_PAINT", "paint");
        lexer.add_token("KEYWORD_FILL", "fill");
        lexer.add_token("KEYWORD_PATH", "path");

        // - Entidades y Restricciones -
        lexer.add_token("KEYWORD_LIMIT", "limit");
        lexer.add_token("KEYWORD_SPAWN", "spawn");
        lexer.add_token("KEYWORD_ARRANGE", "arrange");
        lexer.add_token("KEYWORD_DISTRIBUTE", "distribute");
        lexer.add_token("KEYWORD_SCATTER", "scatter");

        // - Parametros y modificadores posicionales -
        lexer.add_token("KEYWORD_AT", "at");
        lexer.add_token("KEYWORD_AROUND", "around");
        lexer.add_token("KEYWORD_ON", "on");
        lexer.add_token("KEYWORD_FROM", "from");
        lexer.add_token("KEYWORD_TO", "to");
        lexer.add_token("KEYWORD_CENTER", "center");
        lexer.add_token("KEYWORD_PERIMETER", "perimeter");
        lexer.add_token("KEYWORD_COUNT", "count");
        lexer.add_token("KEYWORD_RADIUS", "radius");

        // ________________________________________________________________________
        // 3 operadores y signos de puntuacion
        // ________________________________________________________________________
        // Menor o igual (<=) antes que cualquier operador simple ***
        lexer.add_token("OP_LESS_EQUAL", "<=");
        lexer.add_token("OP_ASSIGN", "=");

        // Delimitadores y puntuacion
        lexer.add_token("PUNCT_LBRACE", "\\{");
        lexer.add_token("PUNCT_RBRACE", "\\}");
        lexer.add_token("PUNCT_LPAREN", "\\(");
        lexer.add_token("PUNCT_RPAREN", "\\)");
        lexer.add_token("PUNCT_COMMA", ",");
        lexer.add_token("PUNCT_COLON", ":");
        lexer.add_token("PUNCT_SEMICOLON", ";");

        // ________________________________________________________________________
        // 4. Literales Numericos          **FLOAT_LITERAL antes que INT_LITERAL para reconocer '2.0' como float
        // ________________________________________________________________________
        const std::string DIGITS_PLUS = "(" + DIGIT + ")(" + DIGIT + ")*";
        lexer.add_token("FLOAT_LITERAL", DIGITS_PLUS + "\\." + DIGITS_PLUS);
        lexer.add_token("INT_LITERAL", DIGITS_PLUS);

        // ________________________________________________________________________
        // 5. Literales de cadena y colores hexadecimales ("...")
        // ________________________________________________________________________
        const std::string STR_INNER =
            "(" + ALPHANUM + "|#|/|\\.|-|_| )";
        lexer.add_token("STRING_LITERAL", "\"" + STR_INNER + "*\"");

        lexer.add_token("ID", LETTER + ALPHANUM + "*");

        // ________________________________________________________________________
        // 6 comentarios y espacios en blanco y/o descartables
        // ________________________________________________________________________
 
        const std::string COMMENT_CHAR =
            "(" + ALPHANUM + "| |#|/|\\.|-|:|,|;|\\(|\\)|\\{|\\}|<|>|=|\\+|\\*|'|\")";
        lexer.add_token("COMMENT", "//" + COMMENT_CHAR + "*");

        // Espacios en blanco, tabulaciones y saltos de linea
        lexer.add_token("WS", "( |\t|\n|\r)+");

        return lexer;
    }

    /**
     * Calcula la linea y columna 1-based a partir de una posicion offset.
     */
    inline void calculate_line_column(const std::string& input, nat_t pos, nat_t& out_line, nat_t& out_col)
    {
        out_line = 1;
        out_col = 1;

        nat_t limit = pos < input.size() ? pos : input.size();
        for ( nat_t i = 0; i < limit; ++i )
        {
            if (input[i] == '\n')
            {
                out_line++;
                out_col = 1;
            }
            else
            {
                out_col++;
            }
        }
    }

    /**
     * Procesa una cadena de codigo fuente en GeoGrid3D y devuelve la secuencia de tokens enriquecida con fila y columna.
     * @param input Codigo fuente GeoGrid3D
     * @param filter_trivia Si es true, descarta tokens de espacios (WS) y comentarios (COMMENT).
     */
    inline DynArray<GeoGridToken> tokenize_geogrid(const Lexer& lexer,
                                                  const std::string& input,
                                                  bool filter_trivia = true)
    {
        DynArray<Token> raw_tokens;
        try
        {
            raw_tokens = lexer.tokenize(input);
        }
        catch (const std::exception& ex)
        {
            std::string msg = ex.what();
            size_t p = msg.find("position ");
            if (p != std::string::npos)
            {
                size_t start = p + 9;
                size_t end = msg.find(" ", start);
                if (end != std::string::npos)
                {
                    nat_t err_pos = std::stoul(msg.substr(start, end - start));
                    nat_t err_line = 1, err_col = 1;
                    calculate_line_column(input, err_pos, err_line, err_col);
                    throw std::runtime_error("Fallo lexico en Linea " + std::to_string(err_line) +
                                             ", Columna " + std::to_string(err_col) +
                                             " (offset " + std::to_string(err_pos) + "): Caracter no reconocido '" +
                                             input[err_pos] + "'");
                }
            }
            throw;
        }

        DynArray<GeoGridToken> tokens;

        for (nat_t i = 0 ; i < raw_tokens.size() ; ++i)
        {
            const Token& rt = raw_tokens[i];

            if (filter_trivia && (rt.name == "WS" || rt.name == "COMMENT"))
            {
                continue;
            }
            nat_t line = 1, col = 1;
            calculate_line_column(input, rt.position, line, col);
            tokens.append(GeoGridToken{rt.name, rt.lexeme, rt.position, line, col});
        }

        return tokens;


    }
}
