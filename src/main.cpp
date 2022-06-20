#include <cstdlib>
#include <string>

#include <kformat.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

// std::system("clang++ -Wall -Wno-unused file.cpp -o out")

namespace {
struct Rpc {
	static constexpr std::string_view dev_null_v =
#if defined(_WIN32)
		"NUL";
#else
		"/dev/null";
#endif

	static std::string routeNull(std::string_view const command) { return ktl::kformat("{} > {} 2>&1", command, dev_null_v); }

	int returnCode{};

	Rpc(char const* command, bool silent = false) {
		if (silent) {
			returnCode = std::system(routeNull(command).c_str());
		} else {
			returnCode = std::system(command);
		}
	}

	bool successful() const { return returnCode == 0; }
	explicit operator bool() const { return successful(); }
};

struct Compiler {
	std::string_view path{};
	std::string_view options{};

	bool available() const { return Rpc(ktl::kformat("{} --version", path).c_str(), true).successful(); }
	bool compile(char const* in, char const* out) const { return Rpc(ktl::kformat("{} {} {} -o {}", path, options, in, out).c_str()).successful(); }
};

struct Cli {
	char caret{'>'};
	char meta{'$'};

	std::string get() {
		std::printf("%c ", caret);
		auto ret = std::string{};
		std::getline(std::cin, ret);
		return ret;
	}

	void put(char const* text) {
		if (!*text) { return; }
		std::printf("%s\n", text);
	}
};

struct Source {
	std::string cpp{"repl.cpp"};
	std::string exe{"repl"};

	std::string code = R"(#include <iostream>
#include <string>
#include <vector>

int main() {
)";

	bool append(Compiler compiler, std::string line) {
		auto file = std::ofstream(cpp);
		if (!file) { return false; }
		auto copy = code;
		if (!line.empty()) {
			copy += line;
			copy += '\n';
		}
		file << copy << "\n}\n";
		file.close();
		if (!compiler.compile(cpp.c_str(), exe.c_str())) { return false; }
		code = std::move(copy);
		return true;
	}

	void execute() const {
		if (std::filesystem::is_regular_file(exe)) {
			Rpc rpc(std::filesystem::absolute(exe).c_str());
			std::puts("");
		}
	}
};

struct Context {
	Compiler compiler{};
	Cli cli{};
	Source source{};

	bool meta(std::string_view const cmd) {
		if (cmd == "quit") { return false; }
		return true;
	}

	void loop() {
		while (true) {
			auto line = cli.get();
			if (line[0] == cli.meta) {
				if (!meta(line.substr(1))) { return; }
			} else {
				// compile, run / print
				if (source.append(compiler, std::move(line))) { source.execute(); }
			}
		}
	}
};
} // namespace

int main() {
	auto ctx = Context{Compiler{"clang++"}};
	if (!ctx.compiler.available()) {
		std::fprintf(stderr, "%s\n", ktl::kformat("{} compiler not available", ctx.compiler.path).c_str());
		return EXIT_FAILURE;
	}
	ctx.loop();
}
