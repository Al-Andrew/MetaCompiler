import util

import argparse
import os
import sys
import time
import logging
import enum

import coloredlogs

class Exit_Codes(enum.Enum):
    SUCCESS = 0
    NO_TESTS_TO_RUN = 1
    TESTS_FAILED = 2


def init_logging() -> None:
    coloredlogs.install(
        level='DEBUG',
        fmt='%(asctime)s [%(levelname)s] %(message)s',
        datefmt='%H:%M:%S'
    )


class Config():
    path_to_mc_exe = None
    tests_path = None
    tests_to_run = None
    tests_number_to_path_map = None

    @staticmethod
    def init() -> None:
        ap = argparse.ArgumentParser(description='Test runner for MetaCompiler')
        ap.add_argument('path_to_mc_exe', type=str, help='Path to MetaCompiler executable')
        ap.add_argument('--tests-dir', type=str, default='.', help='Path to tests directory (default: .)')
        gr = ap.add_mutually_exclusive_group()
        gr.add_argument('--all', action='store_true', help='Run all tests')
        gr.add_argument('--tests', type=str, nargs='+', help='Run specified tests by number')
        
        args = ap.parse_args()
        Config.path_to_mc_exe = args.path_to_mc_exe
        Config.tests_path = args.tests_dir
        tests_available, tests_number_to_path_map = util.get_availalbe_tests(args.tests_dir)
        Config.tests_number_to_path_map = tests_number_to_path_map
        if args.all:
            Config.tests_to_run = tests_available
        elif args.tests:
            Config.tests_to_run = [int(test_number) for test_number in args.tests]
        else:
            logging.critical('No tests specified!')
            sys.exit(Exit_Codes.NO_TESTS_TO_RUN.value)


class Assertion_Result():
    def __init__(self, assertion_number: int, assertion_name: str, success: bool) -> None:
        self.assertion_number:int = assertion_number
        self.assertion_name:str = assertion_name
        self.success:bool = success


class Test_Result():
    def __init__(self, test_number: int, test_full_name: str, assertions: list[Assertion_Result]) -> None:
        self.test_number = test_number
        self.test_full_name = test_full_name
        self.assertions = assertions

    @property
    def success(self) -> bool:
        return all([assertion.success for assertion in self.assertions])


def run_tests() -> list[Test_Result]:
    test_results = []
    
    try:
        os.mkdir(os.path.join(Config.tests_path, '.outputs'))
    except FileExistsError:
        pass
    
    for test_number in Config.tests_to_run:
        test_path = Config.tests_number_to_path_map[test_number]
        test_results.append(run_test(test_number, test_path))

    return test_results


def run_test(test_number: int, test_path: str) -> Test_Result:
    logging.info('Running test #%d...', test_number)
    test_full_name = os.path.basename(test_path)
    test_outputs_dir_path = os.path.join(Config.tests_path, '.outputs', test_full_name)

    inputs_dir_path = os.path.join(test_path, 'inputs')
    expected_outputs_dir_path = os.path.join(test_path, 'expected')
    assertion_results = []

    try:
        os.mkdir(test_outputs_dir_path)
    except FileExistsError:
        pass
    
    for assertion_number, assertion_name in enumerate(os.listdir(inputs_dir_path)):
        assertion_input_path = os.path.join(inputs_dir_path, assertion_name)
        assertion_expected_output_path = os.path.join(expected_outputs_dir_path, assertion_name)
    
        assertion_results.append(run_assertion(test_outputs_dir_path, assertion_input_path, assertion_expected_output_path))
    
    return Test_Result(test_number, test_full_name, assertion_results)


def run_assertion(test_output_path: str, assertion_input_path: str, assertion_expected_output_path: str) -> Assertion_Result:
    assertion_number = int(os.path.basename(assertion_input_path).split('_')[0])
    logging.info('    Running assertion: #%d', assertion_number)

    assertion_output_path = os.path.join(test_output_path, os.path.basename(assertion_input_path))
    assertion_name = os.path.basename(assertion_input_path)
    assertion_success = False

    try:
        os.mkdir(assertion_output_path)
    except FileExistsError:
        pass

    assertion_stdout_path = assertion_output_path + '.stdout'
    assertion_stderr_path = assertion_output_path + '.stderr'
    assertion_code_path = assertion_output_path + '.code'

    start_time = time.time()

    # TODO (AAL):
    #os.system(f'{Config.path_to_mc_exe} {assertion_input_path} -o {assertion_code_path} > {assertion_input_path}.stdout 2> {assertion_input_path}.stderr')

    end_time = time.time()
    assertion_time = end_time - start_time

    assertion_success = True # TODO (AAL): util.compare_files(assertion_code_path, assertion_expected_output_path)
    logging.info('        Assertion #%d %s', assertion_number, 'passed' if assertion_success else 'failed')
    logging.info('        Assertion #%d time: %.3f', assertion_number, assertion_time)

    return Assertion_Result(assertion_number, assertion_name, assertion_success)


def main() -> None:
    init_logging()
    Config.init()
    logging.info('Running tests...')
    logging.info('MetaCompiler executable: %s', Config.path_to_mc_exe)
    logging.info('Tests to run: %s', Config.tests_to_run)
    logging.info('Tests number to path map: %s', Config.tests_number_to_path_map)
    logging.info('')
    test_results = run_tests()


if __name__ == '__main__':
    main()