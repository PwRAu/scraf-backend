/* Malisssssimo Luca, i test li devi fare in base a quello che dev'essere fatto,
 * non quello che è stato implementato finora;
 * non ti deve interessare di quello che c'è a livello di implementazione.
TEST(PutStudents, PutMethodOnApi){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};
	std::jthread servingThread {[&]() {
		scraf.serve();
	}};
	EXPECT_EQ(
		cpr::Put(
			cpr::Url{"localhost:" + std::to_string(port) + "/students"},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::Body{json{
				{"mail", "andrea@pappacoda.it"},
				{"password", "coconutnut1968"},
				{"name", "Andrea"},
				{"surname", "Pappacoda"}
			}.dump()}
		).status_code, 
		static_cast<std::int32_t>(Http::Code::Method_Not_Allowed)); //la richiesta effettuata con Put non dovrebbe essere permessa perchè il metodo non è ancora stato implementato
	scraf.shutdown();
}
*/
