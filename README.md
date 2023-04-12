# Пример первого домашнего задания по курсу Архитектура программных систем

Обращаю внимание что в сервисах нужно реализовтаь схему простой аутентификации (в данном случае) other_service - выполняет роль стороннего сервиса, а сервис hl_mai_lab_01 - сервиса который работает с данными пользователей и осуществляет аутентификацию
```plantuml
@startuml
!include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Container.puml


Person(user, "Пользователь")

Container(user_service, "Сервис работы с пользователем", "C++")    
Container(other_service, "Сервис делающий что-то", "C++") 

Rel(user, user_service, "Управлять пользователям")
Rel(user, other_service, "Сделать что-то полезное")
Rel(other_service,user_service, "/auth - проверка логина/пароля")

@enduml
```
