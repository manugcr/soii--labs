package models

type DashboardData struct {
	Offers  []Offer `json:"offers"`
	Orders  []Order `json:"orders"`
	Balance int     `json:"balance"`
}

type DashboardResponse struct {
	Code    string        `json:"code"`
	Message DashboardData `json:"message"`
}
