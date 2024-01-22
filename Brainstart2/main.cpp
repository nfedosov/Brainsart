#include "stdafx.h"

#include "signalplotwin.h"
#include <stdio.h>
#include <QTextCodec>
#include <QApplication>

typedef void (*ArgumentHandler)(PCHAR value, ApplicationConfiguration* pConfig);

void ArgumentHandler_Config(PCHAR value, ApplicationConfiguration* pConfig)
{
    // В качестве аргумента пришло имя конфигурационного файла
    //     
    std::ifstream file(value);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << std::endl;
        return;
    }

    std::string line;

    // Read a line from the file
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        double value;
        while (ss >> value) {
            pConfig->values.push_back(value);
            // Check if the next character is a comma and skip it
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }

    }

    file.close();


    pConfig->numberOfChannes = pConfig->values[0];

    pConfig->filterType = pConfig->values[1];

    
}

void ArgumentHandler_Out(PCHAR value, ApplicationConfiguration* pConfig)
{
    // В качестве аргумента пришло имя файла, куда необходимо сохранить полученные данные
    pConfig->fileToSave.assign(value);
}

void ArgumentHandler_User(PCHAR value, ApplicationConfiguration* pConfig)
{
    pConfig->userName.assign(value);
}

void ArgumentHandler_LslStramName(PCHAR value, ApplicationConfiguration* pConfig)
{
    std::vector<stream_info> results = resolve_streams();
    
    for (auto& result : results) {

        if (0 == result.name().compare(value))
        {
            pConfig->lslStreamInfo = result;
            break;
        }
    }
}

#define ELEMENTS_IN(_x_) (sizeof(_x_) / sizeof((_x_)[0]))

#define CHARS_IN(_x_) (ELEMENTS_IN(_x_) - 1)

#define MIN(_a_, _b_) (_a_ > _b_ ? _b_ : _a_)

struct ArgumentDefinition
{
    CHAR shortKey;

    const CHAR* longKey;

    int longKeyLength;

    ArgumentHandler argumentHandler;

} g_ArgumentsTable[] = {
    {'c', "config", CHARS_IN("config"), ArgumentHandler_Config},
    {'o', "out",    CHARS_IN("out"),    ArgumentHandler_Out},
    {'u', "user",   CHARS_IN("user"),   ArgumentHandler_User},
    {'l', "lsl",    CHARS_IN("lsl"),    ArgumentHandler_LslStramName},
};


void ProcessArguments(int argc, char* argv[], ApplicationConfiguration* pConfig)
{
    for (int i = 1; i < argc; i++)
    {
        int argLength = strlen(argv[i]);

        if ('-' == argv[i][0])
        {
            if ('-' == argv[i][1])
            {
                // Разбираем длинный ключ
                for (int arg = 0; arg < ELEMENTS_IN(g_ArgumentsTable); arg++)
                {
                    if (0 == strncmp(
                        &argv[i][2], 
                        g_ArgumentsTable[arg].longKey, 
                        MIN(g_ArgumentsTable[arg].longKeyLength, argLength - 2)))
                    {
                        // Определить, значение ключа лежит в томже параметре,или отдельно
                        if (argLength > g_ArgumentsTable[arg].longKeyLength + 2)
                        {
                            g_ArgumentsTable[arg].argumentHandler(argv[i] + 2 + g_ArgumentsTable[arg].longKeyLength, pConfig);
                        }
                        else
                        {
                            g_ArgumentsTable[arg].argumentHandler(argv[i + 1], pConfig);
                            i++;
                        }

                        break;
                    }
                }
            }
            else
            {
                // Разбираем короткий ключ
                for (int arg = 0; arg < ELEMENTS_IN(g_ArgumentsTable); arg++)
                {
                    if (argv[i][1] == g_ArgumentsTable[arg].shortKey)
                    {
                        // Определить, значение ключа лежит в томже параметре,или отдельно
                        if (argLength > 2)
                        {
                            g_ArgumentsTable[arg].argumentHandler(&argv[i][2], pConfig);
                        }
                        else
                        {
                            g_ArgumentsTable[arg].argumentHandler(argv[i + 1], pConfig);
                            i++;
                        }

                        break;
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QApplication a(argc, argv);

    ApplicationConfiguration* pConfig = new ApplicationConfiguration();
    if (!pConfig)
    {
        return -1;
    }

    if (argc > 1)
    {
        // Разобрать входные параметры
        ProcessArguments(argc, argv, pConfig);

        MainWindow w;
        w.Init(pConfig);
        w.show();

        return a.exec();
    }

    return -1;
}
