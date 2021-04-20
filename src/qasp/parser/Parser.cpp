/*                                                                      
 * GPL3 License 
 *
 * Author(s):                                                              
 *      Antonino Natale <antonio.natale97@hotmail.com>  
 * 
 * 
 * Copyright (C) 2021 Antonino Natale
 *
 * This file is part of qasp.  
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "Parser.hpp"
#include "ParserException.hpp"
#include <iostream>
#include <fstream>
#include <tuple>
#include <iterator>
#include <string>
#include <sstream>
#include <filesystem>

using namespace qasp::parser;


struct Token {

    char tk_type;
    char tk_value;
    uint32_t tk_line;
    uint32_t tk_column;
    const std::string tk_source;

    Token(char type, char value, uint32_t line, uint32_t column, const std::string& source)
        : tk_type(type)
        , tk_value(value)
        , tk_line(line)
        , tk_column(column)
        , tk_source(source) {}

};



std::vector<Program> Parser::parse() const {

    std::vector<Token> tokens;
    std::vector<Program> programs;


    auto tokenize = [&](std::istream& fd, const std::string& source) {

        LOG(__FILE__, INFO) << "Reading source from " << source << std::endl;

        
        tokens.emplace_back(TK_ANNOTATION, 0, 0, 0, source);
        tokens.emplace_back(TK_SOURCE, 'c', 0, 0, source);
        tokens.emplace_back(TK_SOURCE, 'o', 0, 0, source);
        tokens.emplace_back(TK_SOURCE, 'm', 0, 0, source);
        tokens.emplace_back(TK_SOURCE, 'm', 0, 0, source);
        tokens.emplace_back(TK_SOURCE, 'o', 0, 0, source);
        tokens.emplace_back(TK_SOURCE, 'n', 0, 0, source);
        tokens.emplace_back(TK_SOURCE, '\n', 0, 0, source);


        uint32_t line = 1;
        uint32_t column = 1;


        while(fd.good()) {

            char ch;
            switch((ch = fd.get())) {

                case EOF:
                    break;

                case '%':

                    if(fd.good() && fd.get() == '@') {
                     
                        tokens.emplace_back(TK_ANNOTATION, 0, line, column++, source);

                        LOG(__FILE__, TRACE) << "<LEXER> Found annotation"
                                             << " in " << source
                                             << " at " << line << ":" << column << std::endl;

                    } else {

                        while(fd.good() && fd.get() != '\n')
                            ;
                        
                        ch = '\n';

                    }

                    break;

                default:
                    tokens.emplace_back(TK_SOURCE, ch, line, column, source);
                    break;
                
            }

            column++;

            if(ch == '\n') {
                column = 1;
                line++;
            }
                
        }

    };


    for(const auto source : this->sources()) {

        if(source != "-") {

            std::ifstream fd(source, std::ifstream::in);
        
            if(fd.fail()) {

                LOG(__FILE__, ERROR) << "Error reading " << source << std::endl;

                throw std::invalid_argument(source);

            }

            tokenize(fd, source);

        } else {
            tokenize(std::cin, "<STDIN>");
        }

    }



    for(auto it = tokens.begin(); it != tokens.end(); it++) {


        #define EXPECT(it, token)       \
            ((*(it)).tk_type == token)

        #define VALUE(it)               \
            ((*(it)).tk_value)

        #define GOOD(it)                \
            ((it) != tokens.end())

        

        switch((*it).tk_type) {
            
            case TK_ANNOTATION: {

                std::stringstream identifier;
                std::stringstream source;

#ifdef DEBUG
                auto begin = it;
#endif

                while(GOOD(++it) && EXPECT(it, TK_SOURCE) && isalpha(VALUE(it))) {
                    identifier << VALUE(it);
                }

                if(identifier.str().size() == 0)
                    break;


                if(VALUE(it) != '\n') {
                   
                    while(GOOD(++it) && EXPECT(it, TK_SOURCE) && isspace(VALUE(it)) && VALUE(it) != '\n') {
                        // Just skip...
                    }

                    if(VALUE(it) != '\n')
                        break;
                }


                while(GOOD(++it) && !EXPECT(it, TK_ANNOTATION)) {
                    source << VALUE(it);
                }

                it--;



#ifdef DEBUG
                LOG(__FILE__, TRACE) << "<PARSER> Found annotation with identifier: " << identifier.str()
                                     //<< " and source: \n" << source.str() << "\n"
                                     << " in " << (*begin).tk_source
                                     << " at " << (*begin).tk_line << ":" << (*begin).tk_column << std::endl;
#endif

                
                if(identifier.str() == ANNOTATION_COMMON)
                    programs.emplace_back(source.str(), ProgramType::TYPE_COMMON);
                else if(identifier.str() == ANNOTATION_EXISTS)
                    programs.emplace_back(source.str(), ProgramType::TYPE_EXISTS);
                else if(identifier.str() == ANNOTATION_FOREACH)
                    programs.emplace_back(source.str(), ProgramType::TYPE_FOREACH);
                else if(identifier.str() == ANNOTATION_CONSTRAINTS)
                    programs.emplace_back(source.str(), ProgramType::TYPE_CONSTRAINTS);
                else {

#ifdef DEBUG
                    LOG(__FILE__, ERROR) << "Unexpected annotation: " << identifier.str()
                                         << " in " << (*begin).tk_source
                                         << " at " << (*begin).tk_line << ":" << (*begin).tk_column << std::endl;
#endif

                }


            }


            default:
                continue;

        }

        
        throw ParserException((*it).tk_source, (*it).tk_line, (*it).tk_column, VALUE(it)); 

    }

    return programs;

}