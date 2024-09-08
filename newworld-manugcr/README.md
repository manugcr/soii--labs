# Objetive

The objetive of this PR is to merge the final version of this golang API with a CPP server, and a SQL database.

## Description

This PR contains the final version of the API, with the following features:

- [x] Goland API with requested endpoints
- [x] GORM integration with a SQL database
- [x] CPP server to handle the requests
- [x] Dockerfile to build the API
- [x] Traefik to handle the requests
- [x] Swagger to document the API and test it
- [x] JWT to handle the authentication
- [x] Frontend of the API
- [x] Docker-compose to run the project

All the endpoints are working as expected, and the CPP server is able to handle the requests.

To run the project, you need to have Docker and Docker-compose installed in your machine. And the server.cpp image.

```bash
sudo docker-compose up --build --remove-orphans
```

To use and access the api you can use this ports:

```
Swagger:            http://localhost:3000/swagger/
Traefik Dashboard:  http://localhost:8080/dashboard/
Frontend:           http://localhost:8082/root/
```

You can see the database and the tables created in the front end or use psql command:

```bash
psql postgres://postgres:postgres@localhost:5432/postgres
```

---

## Project structure

### Whats an API?

API stands for Application Programming Interface. It is a set of rules that allows one piece of software application to talk to another. It defines the methods for communication between the two software components.

### Why GORM and not plain SQL?

It provides a simple and clean way to interact with a database. It is a powerful tool that allows you to interact with a database without writing SQL queries providing more flexibility and control over the database, without the need to write complex SQL queries, using already implemented methods.

### What is the CPP server function?

The CPP server is a server that handles the requests from the API. It is a simple server that receives the requests and sends them to the API. It is a simple server that is able to handle the requests and send them to the API.

This server also provides extra functions and uses for the refugee market, like the ability to send emergencies and alerts to the users connected, or images with a canny edge filter over sockets and zip files.

The server is the one who is in charge of the front end, and the API is the one who is in charge of the backend.

### What is Traefik?

Traefik is a modern HTTP reverse proxy and load balancer that makes deploying microservices easy. It is a powerful tool that allows you to manage and control the traffic between the services, providing a simple and clean way to handle the requests.

### What is JWT?

JWT is a standard that defines a compact and self-contained way for securely transmitting information between parties as a JSON object. This information can be verified and trusted because it is digitally signed. JWTs can be signed using a secret or a public/private key pair. It is a powerful tool that allows you to handle the authentication and authorization of the users.

### What is swagger?

Swagger is a powerful tool that allows you to document and test your API. It provides a simple and clean way to interact with the API, providing a simple and clean way to test the endpoints.

On this version swagger is the key component to manage all the functionalities of the API, and it is the main tool to test the endpoints.

-------------------------------------------------------

## Example of usage

Once the project is running on docker with the command:

```bash
sudo docker-compose up --build --remove-orphans
```

<p align="center">
  <img src="https://github.com/user-attachments/assets/e0afecdb-c851-487c-8188-c4964fb2dc44"><br>
  <em>Gif 1. API running</em>
</p>

---

## Improved security in passwords and jwt
- [x] Removed plain text password from database
- [x] Removed plain text JWT token from database
- [x] Added bcrypt to log in

The previous version saved the password in plain text, and unnecessarily saved the jwt token in the database. Now bcrypt is used to encrypt the password and store the encrypted password in the database.

```bash
postgres=# select * from users;
 username |        email        |                           password                           | role 
----------+---------------------+--------------------------------------------------------------+------
 johndoe  | johndoe@example.com | $2a$10$2/AA8B5iOcBRm7fdlMuote63H.W0ZG2i4KJB33xDjI6QuX7CWtrt2 | user
```
To enter instead of comparing textually the saved password with the entered one, now the hashes are compared using bcrypt.

```golang
// Compare the hashed password with the provided password
err = bcrypt.CompareHashAndPassword([]byte(user.Password), []byte(loginReq.Password))
    if err != nil {
        return "", errors.New("incorrect password")
}
```
