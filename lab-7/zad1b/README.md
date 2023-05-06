1. Krzesla i poczekalnia to kolejki FIFO
1. Poczekalnia oprocz semafora ma rowniez pamiec wspolna
1. Klient po dolaczeniu sprawdza czy jest miejsce w poczekalni i jeśli jest to do niej dołącza, ustawia pamiec wspolna poczekalni na rodzaj fryzury i czeka az semafor w tej poczekalni jest rowny 0
1. Fryzjer w wolnej chwili sprawdza ilosc klientow w poczekalni, wolne krzeslo, jesli jest wolne to wyciaga rodzaj fryzury i ustawia pamiec wspolna miejsca w poczekalni na id fotelu, ustawia semafor krzesla na 1 i budzi klienta
1. Klient dolacza do semafora krzesla, czeka na koniec strzyzenia i sie konczy 
