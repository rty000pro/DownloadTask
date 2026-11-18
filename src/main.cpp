#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <functional>

void asyncHTTPGet(std::string url, 
    std::function<void(std::string)> callback) {

    std::thread(
        [url, callback]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            callback("200 OK: " + url);
        }
    ).detach();
}

class DownloadTask : public std::enable_shared_from_this<DownloadTask> {
private:
    std::string _url;
    int _id;

    DownloadTask(int id, std::string url) : _id{id}, _url{url} {}
public:
    //Фабричная функция
    static std::shared_ptr<DownloadTask> fabric_create(int id, std::string url) {
        std::cout << "Create Task - " << id << " " << url << std::endl;
        return std::shared_ptr<DownloadTask>(new DownloadTask(id, url));
    }

    ~DownloadTask() {
        std::cout << "Destructor" << std::endl;
    }

    void start() {
        auto self = shared_from_this();//Если это не использовать то оно просто сломается и сегментируется

        for(int i = 0;i != 3;++i)
        {
            asyncHTTPGet(
                _url,
                [self](std::string response) {
                    if(response == "500 Error")
                    {
                        std::cout << "Error 500\n";
                        return 1;
                    }
                    else if (response == "500 Error")
                    {
                        std::cout << self->_id << ' ' << self->_url << ' ' + response << "\n";
                        return 0;
                    }
                    else
                    {
                        std::cout << "Unknown error\n";
                        return 1;
                    }
                }
            );
        }
        //~DownloadTask Вызвался где-то здесь
        //std::cout << "Error 500! Connection failed" << std::endl;
    }
};

class DownloadManager
{
private:
    std::vector<std::shared_ptr<DownloadTask>> VecTaskDownload;
public:
    void addTask(int id, std::string url)
    {
        std::cout << "Add task - " << id << " " << url << "\n";
        VecTaskDownload.push_back(DownloadTask::fabric_create(id,url));
    }
    void startAll()
    {   
        std::cout << "Start all task:\n";
        for(auto x : VecTaskDownload)
        {
            x->start();
        }
    }
};

int main() {
    {
        DownloadManager first;
        first.addTask(1,"test");
        first.addTask(2,"test3");
        first.addTask(3,"test2");
        first.startAll();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return 0;
}
/*
::....:::::::;::;:....;:...:::;+;:::;;:;;;;:;::::::;::;;;;:::::::::::.::::::::::::.:::::::::::::::::
:....::::::::::....::::;;;;+;:+;:;::;;;;;:;;;:...:::;::::;::::::::::::::::::::::::::::::::::::::...:
:...:.::::::::....::;;+++++x;;::::::;+++;;:+;:::.::...::;::.:::::::::::::::::::::::::.:::.::::::::::
::::..::::::::.::::;:++++x+;;;;:..:;.;XXX+++;:::;;::::::;:...:::::::::::::::::::::::::::::::::::::..
::::..:::::::::::::::;;:;++;++++:;:;;:+Xx+XX+;::;:;+;;;::::::::....:::::::::::::::::::::::::::::::..
::;::.::::::::::::;;;+;;:+++x++x+:.:::::;:::.:::;;;;;;:;;+;;;;;::::::::::::..:::....::::::::::::::::
::::::::::::::::;;:;;;;;;;;;+;++++:::::+;:::::;::;;++;;;+;;;:;;;:;:::::::::::::.::.::....:::::::::::
::;::..:::::::::;+;;;;++++++;;x+++:::;;:;;:;;;+++++:++x+++;;;;:;;;;;;::;::::::....:::::::::::::;::::
::;::..::::::::;:::;;;+;;;;;++;+++:::;;+;;:;+;++++;;::+;;;;;+++;;++;;;;:::::::.::::::;;;:;;:::::::::
.::.::::::::::;::;::;;;;;;;++;++;;+;;;++xxX+;+;;;;;+++;:;::;;++;;;;;;;::::::;::::::;+:;;;;;;::::::::
:::::..::::::::::;;;;:;;;;;;;;;;+x;;+++;+++XX+;++xxXXXX+;+;;:;:;;;;;;::;;;;::::;;+++++:;;;;;:;:::::.
;;::;:.::::::::::::;:::.:::;:;;;+++::;+;;+XXX$$$$$$$$$X$$$X+;;;;:::;:::::::;+++;+++++;++;::::::.:.:.
::+:.::::::;;::;+;;:::::::;;.::;;;;+++;+X$$$$$$$$$$&$$$$$$XXX++;;++++++;++xxxx+;+;++;;:::::::.....::
;:;:.:::::::;::;;;;;;::;;;;;:;::;;;+X$$$$$$$$$$$XXX$$$$$$$$XXXx+++xXxxx+:;xxx++++;:::....:...:::::::
:;:;:+;:::;:;::XX:+::::;+;+xx+;;;+X$&&&$&&$$$$&&&&&$XX$$$XX$$XXXXX$$Xx++xx+;+::::.::::::::::::::::::
:;:;:.:::::;;::+;XXx;+++$$xxXx++$$$&$&$$$$$$$$$$$$$$$$XXX$$$X$$XXXXxx+;;;:.:;+::::.:::::::::::::::::
;;::;:::;::::;:;XX$&$+XXXXXxxX$$&&&&$&$$$&$$$$$$$$$$$&$$$X$$$X$$X$XX+;:::::.::::;;;:::::;;;:::::..::
::+;::::::;;:::+$XXXX+;++xXX$$$$$&&$&&&&&$$&&&$&$&&&&&$$&&$XXXX$$XXXX+;::::::;:::;;;;;;;;;::::::::::
+;++::::;;;;;;:;$Xx++X$xXX$$$$$$&$$$&$&&&$&$$$&$$$$&$$$$$$$$&$$X$$XXXXx+++;:;::;+++;;;::::::::::::::
:;::;::::::;;;::++x+X$$&&$$$$$&$$$$$$$$$$&$$$$$$&$$$$$&$$$$$$$$$XX$$$XXX+;+++;;;;;:::::::::::::;;;;;
:;:+;.:::::;;;;;;xXX$&$$$$$$$$$$$$$$$$$$$$$&&&$$X$$$$$$$$$$$$X$$$$$$XXXX+;+;;:::::;;;:;;+;:;++++;;;:
;;;;;:;:::;;++xxXXX$XX$XX$$XxxX+xX+xXXXXXXX$$$&$$$$$$$$$$$$$$$$$$$X$$XXX+;;::;;;++++++++++;;;;;;;;;;
;:+;;:.:::+&$$$$X$$$$$XX$Xx++$$+XXXx++Xxx$XXXX$$$$$$$$$$$$$$$$X$$XX$$$XXx;;++++++;;;;;+++;;;++;;;;;;
;;;+;:.:.;$&&&&$$$$$$XX$$$$$X$$++xxX$XXxXXX$XXX$$$$XX$$$$&&$$$$$$$$XX$$XX+++x++xx+;+++++Xx++++++++++
:;:;::...X$$$$$$&&&$XX$$XXX$$$XXX+$X;+$$$$$$$$x+++X$$$$$$$$XX$$$$$$X$$$$XXXxxXxx++++x+++++xxxx++++++
:+:+:.::.x&$&&&&&$XX$XX++X&$$Xx$X$XXX$X$$$$$X$$$X$$$$$&$$$$$XX$$$$$$$$$$XXXXXXXXX++xxXXxxxxxxx++++++
:+:;;;;:.+&$$$$&$X$$$&&$$$&$$$$$X$$$$$$+++X$X+$&&&$$X$$$X$$$$XXX$$$$$$$$XXXXXXXXXXXXXXxx+++xXXXxx+++
;:;;+:.::X$$$$$$$XXxXX;X+++Xx+XxX$x+XXXX$&$XXX+X&&$$$$XX$XXX$$$$$$X$$$$$$XXXxXxxxXXXXXXXXXXXXXxXxx++
;;;;;;:::+X$&&&$XX$X+$XX$$X;&XXXXXxX+;;x++++xX+x$$$$$$$$$$$$$$$$$$$$$$$$X$$XX+xxxxxxXXXXXXXXXXXxxxxx
+::;:;.::+$&$$$$X+x$$X$$&$$$&$$XX$$;+$$$+X+xX$XX$&$X$$XX$XXX$$$$X$$$$$$$$$XXXXXxXXXXXXXXXXXxxxxxxxx+
:+:+::..;xXXxxXX$&&X+x;;+$$XX$XXXX+X&&XXXXXxXXX$&&$$$$XXXXX$$$$$XX$$$$$$$$$$$XXXXXXXXXXXXXXxxxxxxx++
:+;;;:;;;+XXx+x$&X++x$&$XX+;x$&&$+;+XX++x+x$$XX$$$X$$$XXXX$XX$$$$$$$$$$X$$XXXXXXXx;;+XXXXXXXxxxxx+xx
::;;Xx;+:+x+++$$x;+X$$$XX$$$$$XXXx$$+;$$X+;+$$Xxxx+X$$XXX$$$XX$$$$$$$$$XXXXXXXXXXXXXXXXXXXXX+;;;++++
;;+++X;+:+xxxX$++X$$$$&$$&&&$;+:;++++$&&$Xx;x$XXx+++X$XXXXX$XX$$$$$$$$$XXxXXxXXXXXXXXXXXXXXXXXx+++++
;:;;:++::;xX+X$x+xX$$$$$$$$$$Xx$$$X$$$$$$$XXxxX+;;;xXXXXXXX$XX$$$$$$$XXXXx+xxXXXXXXXXXXXXXXXx++++++;
;;.+;;+:::+$$XX;;XXX$$$X$$$Xxx+;+xXXXX$$$$$X+:$X;:+X$XXXXxX$$XX$$$$$XXXXx++XXXXXXXXXXXXXXXXXx++;;;;+
:+;;;:+::;+xXX+;+X$$$X+XX$$$$$$$XXXXXX$$$$XXX+X$xxXX$XXXXXXXXXX$$$$XXXXXXXXXXxxXXXXxXXXXXXXXxx+++;;;
:;::++:;.;++XXXXXXxXxx++x+XXXXXXXXXxxXX$$X+++;X++XXXXxXXx+xXX$$X$XXxx+xxxxXx+xX;:;;+xXXXXXXxx+++;;;:
;:;:;+:;:;;;xX$XxxxxxxXxx+xX++++xXXXXXXX$XX+;+++XXXXx+xxxXXX$X$$XxxxxXxx++::::::;++++x+xx+;;:;::::::
;:;;:++::;;+xX$XXx+;++xXXX$X;;;;:;XX$$XXXXx;;+;;xXXXXxxXxXXX$XXXxx+++xxx++++;++x+;;:::::::::::::::..
:;:....:+;;;+X$$XXx;;;:+;;x+;;;;++;+xXx+;;+;:;;+XXxXXXXXxXXXXXx+Xx++++++;:;;xxXx;:::::::::::::::....
:+;.:.:::;;;+XX$X+++;;:;;;;;;;++xX+;;;;;;;;+++++xxXXXXXXXXX$Xx;xXx++;+++++;:::::::....:::.........::
;x+....:;;:;;;+XXX+;+;;++;;+++;:++++;;;+;;;+++x++xXX$$XXXXXX+++;x;;;++++++++;:::.:.........:::::::::
:+x;.:.;;;;;::;+X$Xx+:+++;;+++;;;;+;;+++;;;;+;++;+xXXXX$XXXx+;;::;;;;+;+;;;;++++;:.::...::::::::;;::
X+++:.:;:;;;:::;XXXX+++;;+;;;;;+;+;;++++;;;+;;;++++XXXXXXXx+;;:;;;;;+++++xx+;;++;::::::::::::::::::;
X;;x:.:::;;;:::;+xxXXX++++;;;;;++++x+xx+;;:;;:;;++XXXXXXXXX+;::;;++++++x++;++;;:::::::::::::::;;;;+x
xX+:::;;;;;;:::;;;;;+xXx+;;;;;;;;;++++;;;;;;;;;;+x++xXxx++;;+;;;;++++++x+++++;;::..::::::::;;;;;;+xx
;Xx:::;+++;:.::+;:;;++xxxx+++++x+xxxx+;;;++++++++xxxxx++;:::::;;+++xx+++;:;;++;:.:::::::;;;;;;;;;xxx
x+;.:;:;;;;:.::;xX+:;++XXXXXXxxXXXXXXx+++xxxxxXxXXxxx+;:::::::;:;;+;++;;+;;+XX::.:::::;;:;;;;;:;+xxx
;:;::;;;;:;:.:;+;;;;:;++Xx+xXxXXXXXXXXX++xxXXXxXxXXx+;::::::::;;;;;;;:;;:;+XX+::::::::;;;;;;;;;+;;;;
$X:.:;;;;::.:::;;+;;;;;;+XXXXXXXXXXXX$XxxXXXXXXXXXx;;:::;;;+++x++;;;;;:;;;xXX;:::...........:.......
+;..::;;;;:.::;:;:;;;;+++X$$$$XXXXXXXXx;;;++xXXXXX+;;:;;;++;;+++++xx+++++;;;+:::....................
+;:.;::::::.::;;;;;;+++;+$$$$XXXXXXXXx;;+++++++xXx;::;;;;;;;+++++++++xXXXXXXXXXXXXx;:.............:.
::..:::::::.:::;;;;;;++xX$XXXx++;;;+x+;+xXXXXxxXX;::;:;;;;+;++++xxxXXXXXXX$$$$X$$$$$$;...........::.
:..:::::;:::::;;++++;;;+X$$XX++;:++xXx+xXXxxx+xX+:;;;+++++;+;;;;xXX$$$$$$$$$$$$$$$$$X;::...:::..::::
:.:;;;;;;::::;;;;;;;+;;;x$$$XX+++XXX+::::::;;::;;;;++++;;;+++++XXXX$$$XX$$XX$$$$$$$X+:::::::::::::::
:::+++;;;:::;;;;;;;;++;+++X$Xx+;+++;:::;;;;;+;;;;;+++++xxx++++++++++XXXXXXXX$XXxXXXX;:::::::::;;::::
:::+;+;;;:::;++;;;+;;++;;;;;;;+::::::;;;;+x+;;+;+;++++xxXXXxxxX++++;;xXXXXxX$XX$$XXX+:;:::::::;;::::
::;;;;;;;:::;;++++;;;;;;;+;;;;;:;::;;;;+++++++++++++++++++xxXx+xxx+xxxxx+xxX$$XX$$$$X;:::::;:;+;;;;:
.:;++;;;::::::;;;;;+;;;;;;;;;;;;;;;;;+;++x+++++++++x+x++xxxxXxxXxXxxx+;;+x$$$$$$$X$$X;::::::;++;:;;:
::;++;;;:::;;:;;;;;++++++++++++++++;+++xxxx+++++++++++x+xxXXXXXxxxxx++;++;;;;xXXXXX$Xx:::::::;;;;:::
::;+++;;:::;;::;+xxxxxxx+xxx+++++++++xXXXx++x++xx+++x+Xx+++xxxXXXx;;;;+++;;;:::;+;:;+X;:::::::::::::
X:;;;++;:::;;;:;+xxXXXXXx++++x++++x+++xxxxxxx+xx+xXxxxXXxx+x++XXx++::::::::::::;:::.::;::::::;;:::::
$$$;;;;::::;::;:+XXXXXXXXXx+++++xXXXxxxxxxxxxxxxxxxxxxXXXxxxx++XX;;;:::.::::;:::::::::::::::::;;;;+;
:+X&+;;::::::;:;;++++x++xxx+++xx+xxXXXXxxxXXxXXxxxXXXXxXXxxxxx+XXX+;:::::::;:::::::::::::::::::;+++;
.:::++;:::::;;;;;;x++x+xxxXXXXXXXXX$$XXXXXXXXXXX$XXXXXXXXXXXXx+xXXXXX+;;++;;;;+;:::::::;;::::::;+++;
;::::+;::::;;;++x;;+xxxxxxXxxxx+xXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXxX$$$$$X+++++X++;;;;::::::::::::;++++
;;;;;;::;;+xx+x+xx+++xxXxXXx+xx++xxxxxxxxxXXXXXXXXXXXXXXXXXXXXXx+X$X$$$$$X+;;;;+;:;;:::::::::::+++++
+;;;;;::;++++;;;;++xxxxxxx++xx++++x+x++xXxxxXXXXXXXXXXXXXXXXXXXXXX+X$$$$$$$$X+;;;+x++;;::::::::++xx+
+++;;;::;::;;++++xxXXXXx++xxxxx+++xxxxx+xXxxxxXxXXXXXXXXXXXXXXXXXx+++xXXXXXX$X+xxx++XXXX+;:::::+xx+;
;;;;;::::::;;+++xxxXxXXXXXxxxxXxxxXxxXXXXXxXXXXxXxxXXXXXXXXXXXxXXXXXx+x+++x+xXxX+++++XXxxx;+x+x+++++
;;;;;;:::::::;;;+x+xX+xxXXXxXxxXXXxxXxxxxxxxxxxxxXXXXXXXXXXXXXXXXXXXxXXxx++++xxX++;+xxxXx++xx+++++;+
;;;;;::::::;;;;;+xxxXxXXxxXXxxXXXXXXXXXxXXxxxxxXxxXXXXXXXXXXXXXXXXXXXXXXXx+++x++x++;;+++;+++++++;:;;
*/