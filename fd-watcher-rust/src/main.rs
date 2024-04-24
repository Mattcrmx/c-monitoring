use fd_watcher_rust::monitoring::get_pid_by_name;

fn main() {
    get_pid_by_name(String::from("chrome"));
}
