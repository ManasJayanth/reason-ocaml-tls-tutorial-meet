open Lwt.Syntax;
open Lwt_unix;
let backlog = 10;
let serverPort = 8000;

let nullAuth = (~host: _, _) => Ok(None);

let serverCert = "./certs/myserver.crt";
let serverKey = "./certs/myserver.pem";

let rec encryptedHandle = (encryptedOutputChannel, encryptedInputChannel) => {
  let* msg = Lwt_io.read_line(encryptedInputChannel);
  let* () =
    Lwt_io.write_line(encryptedOutputChannel, "SERVER also said: " ++ msg);
  encryptedHandle(encryptedOutputChannel, encryptedInputChannel);
};

let rec unEncryptedHandle = (outputChannel, inChannel) => {
  let* () = Lwt_io.write_line(outputChannel, "foobar");
  unEncryptedHandle(outputChannel, inChannel);
};

let rec processConnection = unEncryptedSocket => {
  /* let* (fd, _sockaddr) = accept(unEncryptedSocket); */
  /* let outputChannel = Lwt_io.of_fd(~mode=Lwt_io.output, fd); */
  /* let inputChannel = Lwt_io.of_fd(~mode=Lwt_io.output, fd); */

  let* cert = X509_lwt.private_of_pems(~cert=serverCert, ~priv_key=serverKey);
  let config =
    Tls.Config.server(
      ~reneg=true,
      ~certificates=`Single(cert), // TODO: what is Single?
      ~authenticator=nullAuth, // TODO: authentication feature by TLSv3
      (),
    );
  let* (encryptedLwtIOChannels, _socketAddr) =
    Tls_lwt.accept_ext(config, unEncryptedSocket);
  let (encrypedInputChannel, encryptedOutputChannel) = encryptedLwtIOChannels;
  encryptedHandle(encryptedOutputChannel, encrypedInputChannel);
};

let main = () => {
  let sockaddr = Unix.ADDR_INET(Unix.inet_addr_any, serverPort);
  let socket = socket(PF_INET, SOCK_STREAM, 0);
  setsockopt(socket, SO_REUSEADDR, true);
  let* () = bind(socket, sockaddr);
  listen(socket, backlog);
  processConnection(socket);
};

Lwt_main.run(main());
