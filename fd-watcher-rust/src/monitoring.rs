use std::{fs::{self, read_to_string, File}, path::{Path, PathBuf}};

pub fn get_pid_by_name(name: String) -> i32 {


    for proc_path in fs::read_dir("/proc").unwrap() {
        let candidate_path = match proc_path {
            Err(err) => panic!("couldn't read file: {}", err),
            Ok(path) => path
        };

        if !(candidate_path.file_name() == "." || candidate_path.file_name() == "..") || PathBuf::from(candidate_path).is_dir() {
            // open status file to look a the process name
            let status_file = match fs::read_to_string(candidate_path.path()) {
                Err(err) => panic!("couldn't read file: {}", err),
                Ok(status) => status 
            };
            println!("{:?}", status_file);
        };
    };
    32
}


pub fn count_descriptors_by_pid(pid: i32) -> usize {
    // Logically we could have a process with pid 2^32 hence i32
    fs::read_dir(format!("/proc/{:?}/fd", pid)).unwrap().count()
}

// pub fn count_descriptors_by_name(name: String) -> usize {

// }
