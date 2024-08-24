# Build stage
FROM golang:1.22.1-alpine AS build

WORKDIR /app

# Install swag CLI (este se puede borrar)
RUN apk add --no-cache git \
    && go install github.com/swaggo/swag/cmd/swag@latest

COPY go.mod go.sum ./
RUN go mod download

COPY . ./

# Generate Swagger docs (este se puede borrar)
RUN swag init -g cmd/main.go -o ./docs --parseDependency

RUN go build -o main ./cmd/main.go 

# Final stage
FROM alpine:latest

WORKDIR /root/

COPY --from=build /app/main .
COPY --from=build /app/.env .
COPY --from=build /app/docs /root/docs

EXPOSE 3000

CMD ["./main"]
