import logging
import os

def get_availalbe_tests(tests_dir: str) -> tuple[list[int], dict[int, str]]:
    """
    Searches 'tests_dir' for folders with names like 'xxxx_*' where 'xxxx' is a number.
    Returns a tuple of two lists:
        1. List of test numbers
        2. Dictionary of test numbers to test paths
    """

    tests_number_to_path_map = {}
    tests_availalbe = []
    
    logging.info('Searching for tests in: %s', tests_dir)
    for test_dir in os.listdir(tests_dir):
        if not os.path.isdir(os.path.join(tests_dir, test_dir)):
            logging.info('Skipping non-directory: %s', test_dir)
            continue
    
        if not test_dir.split('_')[0].isdigit():
            logging.info('Skipping non-test directory: %s', test_dir)
            continue
    
        logging.info('Found test directory: %s', test_dir)
        test_number = int(test_dir.split('_')[0])
        tests_number_to_path_map[test_number] = os.path.join(tests_dir, test_dir)
        tests_availalbe.append(test_number)
    
    tests_availalbe.sort()
    
    return tests_availalbe, tests_number_to_path_map

