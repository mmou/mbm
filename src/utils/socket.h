namespace mbm {

	class Socket {
		public:
			Socket();
			void bind(sockaddr_in addr);
			void connect(sockaddr_in addr);

		private:
			int fd;
	}
	
}